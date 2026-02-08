#include "raylib.h"
#include "raytracer.h"

int main(int argc, char **argv) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "Raytracer");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    Vec3 origin = {.x = 0, .y = 0, .z = -1};
    Object objects[] = {{
        .pos_center = {.x = 100, .y = 100, .z = 400},
        .color = BLUE,
        .radius = 30,
    }};
    World world = {.objects = objects, .num_objects = 1};

    trace_rays(screenWidth, screenHeight, &origin, &world);

    EndDrawing();
  }

  return 0;
}
