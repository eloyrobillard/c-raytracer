#include "camera.h"
#include "vec3.h"
#include <raylib.h>

void move_with_y_rotation(Vec3 *move, Vec3 *to_move, double y_rot, double speed) {
  Vec3 unit_move = normalized(move);
  Mat3 rot_mat = rot_y_mat(y_rot);
  vm_mul_inplace(&rot_mat, &unit_move);
  vec3_scalar_mul_inplace(&unit_move, speed);
  vec3_add_inplace(to_move, &unit_move);
}

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

  move_with_y_rotation(&move, &camera->global_position, camera->global_rotation.y, delta * 100);
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
  Vec3 unit_rotation = normalized(&rotation);

  camera->global_rotation.y += rotation.y;
}
