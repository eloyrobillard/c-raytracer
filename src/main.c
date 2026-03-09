#include "raytracer.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define RLIGHTS_IMPLEMENTATION
#include <rlights.h>

#define NUM_SPHERES 2

int main(int argc, char **argv) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;
  const int halfWidth = screenWidth / 2;
  const int halfHeight = screenHeight / 2;

  InitWindow(screenWidth, screenHeight, "Raytracer");
  SetTargetFPS(60);

  Object objects[NUM_SPHERES] = {{
                                     .type = SPHERE,
                                     .pos_center = {.x = 0, .y = 0, .z = 0},
                                     .color = RED,
                                     .radius = 500,
                                 },
                                 {
                                     .type = SPHERE,
                                     .pos_center = {.x = 0, .y = -400500, .z = 0},
                                     .color = GREEN,
                                     .radius = 400000,
                                 }};

  RTLight lights[] = {{.type = DIRECTIONAL, .direction = {.x = 1, .y = -1, .z = 1}, .intensity = 120.0}};

  World world = {.objects = objects, .num_objects = NUM_SPHERES, .lights = lights, .num_lights = 1};

  Shader shader = LoadShader(0, "shaders/ray_f.glsl");
  // ray_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(ray_shader, "mvp");
  shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

  for (int si = 0; si < world.num_objects; si++) {
    Object sphere = world.objects[si];

    int positionLoc = GetShaderLocation(shader, TextFormat("spheres[%i].position", si));
    int radiusLoc = GetShaderLocation(shader, TextFormat("spheres[%i].radius", si));
    int colorLoc = GetShaderLocation(shader, TextFormat("spheres[%i].color", si));

    float spherePos[3] = {(float)sphere.pos_center.x, (float)sphere.pos_center.y, (float)sphere.pos_center.z};
    float sphereColor[3] = {(float)sphere.color.r / 255.0f, (float)sphere.color.g / 255.0f,
                            (float)sphere.color.b / 255.0f};

    SetShaderValue(shader, positionLoc, spherePos, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, radiusLoc, &sphere.radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, colorLoc, sphereColor, SHADER_UNIFORM_VEC3);
  }

  SetShaderValue(shader, GetShaderLocation(shader, "ambient"), (float[3]){0.2f, 0.2f, 0.2f}, SHADER_UNIFORM_VEC3);

  CreateLight(LIGHT_DIRECTIONAL, (Vector3){-1.0f, 1.0f, -1.0f}, (Vector3){0.0f, 0.0f, 0.0f},
              (Color){255.0f, 255.0f, 255.0f, 255.0f}, shader);

  Camera camera = {0};
  camera.position = (Vector3){0.0f, 0.0f, -(float)halfWidth}; // Camera position
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};                // Camera looking at point
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};                    // Camera up vector (rotation towards target)
  camera.fovy = 45.0f;                                        // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;

  const int SPEED = 4;
  while (!WindowShouldClose()) {
    float direction_fb = 0;
    if (IsKeyDown(KEY_S))
      direction_fb += 1;
    if (IsKeyDown(KEY_W))
      direction_fb -= 1;

    UpdateCamera(&camera, CAMERA_ORBITAL);
    double theta = atan2(camera.position.z, camera.position.x);
    camera.position.x += cos(theta) * direction_fb * SPEED;
    camera.position.z += sin(theta) * direction_fb * SPEED;

    // Update the light shader with the camera view position
    float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    Vector2 resolutionVec = {(float)screenWidth, (float)screenHeight};
    SetShaderValue(shader, GetShaderLocation(shader, "resolution"), &resolutionVec, SHADER_UNIFORM_VEC2);

    BeginDrawing();

    ClearBackground(BLACK);

    BeginShaderMode(shader);

    DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);

    EndShaderMode();

    DrawFPS(10, 10);

    EndDrawing();
  }

  UnloadShader(shader);

  CloseWindow();

  return 0;
}
