#include "vec3.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

///////////////////////////////////////////////////////////
/////////////////////// VECTORS ///////////////////////////
///////////////////////////////////////////////////////////

void print_vec3(const Vec3 *v) { printf("x: %f y: %f z: %f\n", v->x, v->y, v->z); }

Vec3 vec3_add(const Vec3 *v1, const Vec3 *v2) {
  Vec3 v = {v1->x + v2->x, v1->y + v2->y, v1->z + v2->z};
  return v;
}

Vec3 *vec3_add_inplace(Vec3 *v1, const Vec3 *v2) {
  v1->x += v2->x;
  v1->y += v2->y;
  v1->z += v2->z;
  return v1;
}

Vec3 vec3_difference(const Vec3 *v1, const Vec3 *v2) {
  Vec3 v = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};
  return v;
}

bool vec3_equal(const Vec3 *v1, const Vec3 *v2) { return (v1->x == v2->x) && (v1->y == v2->y) && (v1->z == v2->z); }
bool vec3_equal_with_error_margin(const Vec3 *v1, const Vec3 *v2, double margin) {
  return (absf(v1->x - v2->x) <= margin) && (absf(v1->y - v2->y) <= margin) && (absf(v1->z - v2->z) <= margin);
}

double vec3_dot(const Vec3 *v1, const Vec3 *v2) { return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z; }

double vec3_magnitude_squared(const Vec3 *v) { return vec3_dot(v, v); }

Vec3 vec3_scalar_mul(const Vec3 *v, double scalar) {
  Vec3 res = {v->x * scalar, v->y * scalar, v->z * scalar};
  return res;
}

Vec3 *vec3_scalar_mul_inplace(Vec3 *v, double scalar) {
  v->x *= scalar;
  v->y *= scalar;
  v->z *= scalar;
  return v;
}

Vec3 vec3_scalar_div(const Vec3 *v, double scalar) {
  Vec3 res = {v->x / scalar, v->y / scalar, v->z / scalar};
  return res;
}

double vec3_magnitude(const Vec3 *v) { return sqrt(vec3_magnitude_squared(v)); }
Vec3 normalized(const Vec3 *v) {
  double mag = vec3_magnitude(v);
  return mag ? vec3_scalar_div(v, mag) : *v;
}

Vec3 vm_mul(Mat3 *mat, Vec3 *v) {
  Vec3 res = {vec3_dot(&mat->_0, v), vec3_dot(&mat->_1, v), vec3_dot(&mat->_2, v)};
  return res;
}

Vec3 *vm_mul_inplace(Mat3 *mat, Vec3 *v) {
  double x = vec3_dot(&mat->_0, v);
  double y = vec3_dot(&mat->_1, v);
  double z = vec3_dot(&mat->_2, v);
  v->x = x, v->y = y, v->z = z;
  return v;
}

Vec3 *vec3_rotate_by_inplace(Vec3 *v, double angle_rad) {
  Mat3 rot_mat = rot_y_mat(angle_rad);
  return vm_mul_inplace(&rot_mat, v);
}

void move_forward(Vec3 *move, Vec3 *mover, double x_rot, double y_rot, double speed) {
  Vec3 unit_move = normalized(move);
  apply_rotation(rot_x_mat, x_rot, &unit_move);
  apply_rotation(rot_y_mat, y_rot, &unit_move);
  vec3_scalar_mul_inplace(&unit_move, speed);
  vec3_add_inplace(mover, &unit_move);
}

void apply_rotation(Mat3 (*rot_f)(double), double rot_rad, Vec3 *rotated) {
  Mat3 rot_mat = rot_f(rot_rad);
  vm_mul_inplace(&rot_mat, rotated);
}

Vec3 rot_x_around_point(Vec3 *point, Vec3 *to_rotate, double angle_rad) {
  Mat3 rotation = rot_x_mat(angle_rad);
  Vec3 pos_to_point = vec3_difference(to_rotate, point);
  vm_mul_inplace(&rotation, &pos_to_point);
  Vec3 to_rotate_global = vec3_add(&pos_to_point, point);
  return to_rotate_global;
}

Vec3 rot_y_around_point(Vec3 *point, Vec3 *to_rotate, double angle_rad) {
  Mat3 rotation = rot_y_mat(angle_rad);
  Vec3 pos_to_point = vec3_difference(to_rotate, point);
  vm_mul_inplace(&rotation, &pos_to_point);
  Vec3 to_rotate_global = vec3_add(&pos_to_point, point);
  return to_rotate_global;
}

///////////////////////////////////////////////////////////
/////////////////////// MATRICES //////////////////////////
///////////////////////////////////////////////////////////

Mat3 rot_x_mat(double angle_rad) {
  Mat3 mat = {{1, 0, 0}, {0, cos(angle_rad), sin(angle_rad)}, {0, -sin(angle_rad), cos(angle_rad)}};
  return mat;
}

Mat3 rot_y_mat(double angle_rad) {
  Mat3 mat = {{cos(angle_rad), 0, -sin(angle_rad)}, {0, 1, 0}, {sin(angle_rad), 0, cos(angle_rad)}};
  return mat;
}
