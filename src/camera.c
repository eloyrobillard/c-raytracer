#include "camera.h"
#include "vec3.h"
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

  Vec3 move = {direction_lr, 0, direction_fb};

  move_forward(&move, &camera->global_position, camera->global_rotation.x, camera->global_rotation.y, delta * 100);
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

  Vec3 rotation = {tilt_v * delta, tilt_h * delta, 0};

  camera->global_rotation.x += rotation.x;
  camera->global_rotation.y += rotation.y;
}
