#include "camera.h"
#include "raytracer.h"
#include <raylib.h>

void move_camera(RTCamera *camera, float delta) {
  float direction_lr = 0;
  if (IsKeyDown(KEY_A))
    direction_lr -= 1;
  if (IsKeyDown(KEY_D))
    direction_lr += 1;

  float direction_fb = 0;
  if (IsKeyDown(KEY_S))
    direction_fb -= 1;
  if (IsKeyDown(KEY_W))
    direction_fb += 1;

  Vec3 move = {direction_lr * delta * 100, 0, direction_fb * delta * 100};
  Vec3 unit_move = normalized(&move);

  camera->global_position.x += move.x;
  camera->global_position.y += move.y;
}

void tilt_camera(RTCamera *camera, float delta) {
  int tilt_h = 0;
  if (IsKeyDown(KEY_LEFT))
    tilt_h += 1;
  if (IsKeyDown(KEY_RIGHT))
    tilt_h -= 1;

  int tilt_v = 0;
  if (IsKeyDown(KEY_DOWN))
    tilt_v += 1;
  if (IsKeyDown(KEY_UP))
    tilt_v -= 1;
}
