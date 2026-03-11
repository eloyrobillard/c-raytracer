#include "raytracer.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define NUM_SPHERES 4
#define NUM_LIGHTS 1

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
                                     .material = DIFFUSE,
                                     .radius = 400,
                                 },
                                 {
                                     .type = SPHERE,
                                     .pos_center = {.x = 1000, .y = 0, .z = 0},
                                     .color = YELLOW,
                                     .material = METAL,
                                     .radius = 400,
                                 },
                                 {
                                     .type = SPHERE,
                                     .pos_center = {.x = -1000, .y = 0, .z = 0},
                                     .color = BLUE,
                                     .material = METAL,
                                     .radius = 400,
                                 },
                                 {
                                     .type = SPHERE,
                                     .pos_center = {.x = 0, .y = -400400, .z = 0},
                                     .color = GREEN,
                                     .material = DIFFUSE,
                                     .radius = 400000,
                                 }};

  Light lights[NUM_LIGHTS] = {{LIGHT_DIRECTIONAL, 1, {-1, 1, -1}, {0, 0, 0}, {1, 1, 1, 1}}};
  World world = {.objects = objects, .num_objects = NUM_SPHERES, .lights = lights, .num_lights = NUM_LIGHTS};

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

    double theta = atan2(camera.position.z, camera.position.x);
    camera.position.x += cos(theta) * direction_fb * SPEED;
    camera.position.z += sin(theta) * direction_fb * SPEED;

    BeginDrawing();

    ClearBackground(BLACK);

    trace_rays(halfWidth, halfHeight, &camera, &world);

    DrawFPS(10, 10);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
