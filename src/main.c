#include "camera.h"
#include "raylib.h"
#include "raytracer.h"

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
                          .pos_center = {.x = 0, .y = 0, .z = 150},
                          .color = RED,
                          .radius = 250,
                      },
                      {
                          .type = SPHERE,
                          .pos_center = {.x = -240, .y = -160, .z = 250},
                          .color = BROWN,
                          .radius = 125,
                      },
                      {
                          .type = SPHERE,
                          .pos_center = {.x = 240, .y = 160, .z = 5},
                          .color = BLUE,
                          .radius = 125,
                      }};

  Light lights[] = {{.type = DIRECTIONAL, .direction = {.x = 1, .y = -1, .z = 1}, .intensity = 120.0}};

  World world = {.objects = objects, .num_objects = 3, .lights = lights, .num_lights = 1};
  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    tilt_camera(&camera, GetFrameTime());
    move_camera(&camera, GetFrameTime());

    trace_rays(halfWidth, halfHeight, &camera, &world);

    EndDrawing();
  }

  return 0;
}
