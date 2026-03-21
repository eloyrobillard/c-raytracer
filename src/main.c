#include "raytracer.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SPHERES 2
#define NUM_LIGHTS 1

int main(int argc, char **argv) {
  const double aspectRatio = 16.0 / 9.0;
  const int imgWidth = 384 * 2;
  const int imgHeight = (int)(imgWidth / aspectRatio);

  const double viewportHeight = 2.0;
  const double viewportWidth = viewportHeight * aspectRatio;

  srand(time(NULL));

  InitWindow(imgWidth, imgHeight, "Raytracer");

  Object objects[NUM_SPHERES] = {{
                                     .type = SPHERE,
                                     .pos_center = {.x = 0, .y = 0, .z = 1.0},
                                     .color = RED,
                                     .material = DIFFUSE,
                                     .radius = 0.5f,
                                 },
                                 // {
                                 //     .type = SPHERE,
                                 //     .pos_center = {.x = 1000, .y = 0, .z = 0},
                                 //     .color = YELLOW,
                                 //     .material = METAL,
                                 //     .radius = 400,
                                 // },
                                 // {
                                 //     .type = SPHERE,
                                 //     .pos_center = {.x = -1000, .y = 0, .z = 0},
                                 //     .color = BLUE,
                                 //     .material = METAL,
                                 //     .radius = 400,
                                 // },
                                 {
                                     .type = SPHERE,
                                     .pos_center = {.x = 0, .y = -100.5, .z = 1.0},
                                     .color = GREEN,
                                     .material = DIFFUSE,
                                     .radius = 100.0f,
                                 }};

  Light lights[NUM_LIGHTS] = {{LIGHT_DIRECTIONAL, 1, {-1, 1, -1}, {0, 0, 0}, {1, 1, 1, 1}}};
  World world = {.objects = objects, .num_objects = NUM_SPHERES, .lights = lights, .num_lights = NUM_LIGHTS};

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
