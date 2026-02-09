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

    Vec3 origin = {.x = 0, .y = 0, .z = -1000};
    Object objects[] = {{
        .pos_center = {.x = 0, .y = 0, .z = 10},
        .color = RED,
        .radius = 2,
    }};
    World world = {.objects = objects, .num_objects = 1};

    trace_rays(halfWidth, halfHeight, &origin, &world);

    EndDrawing();
  }

  return 0;
}
