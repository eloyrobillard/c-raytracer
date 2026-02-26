#include "camera.h"
#include "raytracer.h"
#include <raylib.h>
#include <raymath.h>

#define RLIGHTS_IMPLEMENTATION
#include <rlights.h>

int main(int argc, char **argv) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;
  const int halfWidth = screenWidth / 2;
  const int halfHeight = screenHeight / 2;

  InitWindow(screenWidth, screenHeight, "Raytracer");
  SetTargetFPS(60);

  RTCamera camera = {.global_position = {0, 0, -halfWidth}, .global_rotation = {0, 0, 0}};
  Object objects[] = {{
                          .type = SPHERE,
                          .pos_center = {.x = 0, .y = 0, .z = 0},
                          .color = RED,
                          .radius = 2,
                      },
                      {
                          .type = SPHERE,
                          .pos_center = {.x = 5, .y = 5, .z = 0},
                          .color = BROWN,
                          .radius = 2,
                      },
                      {
                          .type = SPHERE,
                          .pos_center = {.x = -5, .y = -5, .z = 0},
                          .color = BLUE,
                          .radius = 2,
                      }};

  RTLight lights[] = {{.type = DIRECTIONAL, .direction = {.x = 1, .y = -1, .z = 1}, .intensity = 120.0}};

  World world = {.objects = objects, .num_objects = 3, .lights = lights, .num_lights = 1};

  Shader shader = LoadShader(0, "shaders/ray_f.glsl");
  // ray_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(ray_shader, "mvp");
  // ray_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(ray_shader, "viewPos");

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

  Camera cam3d = {0};
  cam3d.position =
      (Vector3){camera.global_position.x, camera.global_position.y, camera.global_position.z}; // Camera position
  cam3d.target = (Vector3){0.0f, 0.0f, 0.0f}; // Camera looking at point
  cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};     // Camera up vector (rotation towards target)
  cam3d.fovy = 45.0f;                         // Camera field-of-view Y
  cam3d.projection = CAMERA_PERSPECTIVE;

  while (!WindowShouldClose()) {
    // UpdateCamera(&cam3d, CAMERA_ORBITAL);

    // Update the light shader with the camera view position
    // float cameraPos[3] = {cam3d.position.x, cam3d.position.y, cam3d.position.z};
    // SetShaderValue(ray_shader, ray_shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

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
