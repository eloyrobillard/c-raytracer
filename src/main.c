#include "raytracer.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SPHERES 4

int main(int argc, char **argv) {
  const double aspectRatio = 16.0 / 9.0;
  const int imgWidth = 384 * 3;
  const int imgHeight = (int)(imgWidth / aspectRatio);

  const double viewportHeight = 2.0;
  const double viewportWidth = viewportHeight * aspectRatio;

  srand(time(NULL));

  InitWindow(imgWidth, imgHeight, "Raytracer");

  Object objects[NUM_SPHERES] = {{
                                     .type = SPHERE,
                                     .pos_center = {.x = 0, .y = 0, .z = 1.0},
                                     .albedo = {230.0 / 255.0, 41.0 / 255.0, 55.0 / 255.0},
                                     .material = DIFFUSE,
                                     .radius = 0.5,
                                 },
                                 {
                                     .type = SPHERE,
                                     .pos_center = {.x = 1.0, .y = 0, .z = 1.0},
                                     .albedo = {253.0 / 255.0, 249.0 / 255.0, 0},
                                     .material = METAL,
                                     .radius = 0.5,
                                     .fuzz = 0.3,
                                 },
                                 {
                                     .type = SPHERE,
                                     .pos_center = {.x = -1.0, .y = 0, .z = 1.0},
                                     .albedo = {0, 121.0 / 255.0, 241.0 / 255.0},
                                     .material = METAL,
                                     .radius = 0.5,
                                     .fuzz = 0.7,
                                 },
                                 {
                                     .type = SPHERE,
                                     .pos_center = {.x = 0, .y = -100.5, .z = 1.0},
                                     .albedo = {0, 228.0 / 255.0, 48.0 / 255.0},
                                     .material = DIFFUSE,
                                     .radius = 100.0,
                                 }};

  World world = {.objects = objects, .num_objects = NUM_SPHERES};

  TCamera camera = {0};
  camera.position = (Vec3){0.0, 0.0, 0.0}; // Camera position
  camera.target = (Vec3){0.0, 0.0, 1.0};   // Camera looking at point
  camera.up = (Vec3){0.0, 1.0, 0.0};       // Camera up vector (rotation towards target)

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

    trace_rays(viewportWidth, viewportHeight, imgWidth, imgHeight, &camera, &world);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
