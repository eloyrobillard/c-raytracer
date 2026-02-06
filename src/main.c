#include "raylib.h"

int main(int argc, char **argv) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "Raytracer");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("SOON: Raytracer here", 700, 500, 40, LIGHTGRAY);

    EndDrawing();
  }

  return 0;
}
