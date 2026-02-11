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

    Vec3 origin = {.x = 0, .y = 0, .z = -screenWidth};
    Object objects[] = {{
                            .type = SPHERE,
                            .pos_center = {.x = 0, .y = 0, .z = 10},
                            .color = RED,
                            .radius = 2,
                        },
                        {
                            .type = SPHERE,
                            .pos_center = {.x = -2, .y = -2, .z = 11},
                            .color = GREEN,
                            .radius = 1,
                        },
                        {
                            .type = SPHERE,
                            .pos_center = {.x = 1, .y = 1, .z = 9},
                            .color = BLUE,
                            .radius = 1,
                        }};

    Light lights[] = {{.type = DIRECTIONAL, .direction = {.x = 1, .y = -1, .z = 1}, .intensity = 100.0}};

    World world = {.objects = objects, .num_objects = 3, .lights = lights, .num_lights = 1};

    trace_rays(halfWidth, halfHeight, &origin, &world);

    EndDrawing();
  }

  return 0;
}
