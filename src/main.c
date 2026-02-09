#include "raylib.h"
#include "raytracer.h"

int main(int argc, char **argv) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;
  const double halfWidth = (double)screenWidth / 2;
  const double halfHeight = (double)screenHeight / 2;

  InitWindow(screenWidth, screenHeight, "Raytracer");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    Vec3 origin = {.x = halfWidth, .y = halfHeight, .z = 0};
    Object objects[] = {{
        .pos_center = {.x = halfWidth, .y = halfHeight, .z = 10},
        .color = BLUE,
        .radius = 100,
    }};
    World world = {.objects = objects, .num_objects = 1};

    trace_rays(screenWidth, screenHeight, &origin, &world);

    EndDrawing();
  }

  return 0;
}
