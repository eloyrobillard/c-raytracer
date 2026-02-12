#include "raylib.h"
#include "raytracer.h"

int main(int argc, char **argv) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;
  const int halfWidth = screenWidth / 2;
  const int halfHeight = screenHeight / 2;

  InitWindow(screenWidth, screenHeight, "Raytracer");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    Vec3 camera = {.x = 0, .y = 0, .z = -screenWidth};
    Object objects[] = {{
                            .type = SPHERE,
                            .pos_center = {.x = 0, .y = 0, .z = 10000},
                            .color = RED,
                            .radius = 2000,
                        },
                        {
                            .type = SPHERE,
                            .pos_center = {.x = -2000, .y = -2000, .z = 15000},
                            .color = GREEN,
                            .radius = 1000,
                        },
                        {
                            .type = SPHERE,
                            .pos_center = {.x = 600, .y = 400, .z = 5000},
                            .color = BLUE,
                            .radius = 500,
                        }};

    Light lights[] = {{.type = DIRECTIONAL, .direction = {.x = 1, .y = -1, .z = 1}, .intensity = 100.0}};

    World world = {.objects = objects, .num_objects = 3, .lights = lights, .num_lights = 1};

    trace_rays(halfWidth, halfHeight, &camera, &world);

    EndDrawing();
  }

  return 0;
}
