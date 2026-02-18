#ifndef VEC3_H
#define VEC3_H
#include <stdbool.h>

typedef struct {
  double x;
  double y;
  double z;
} Vec3;

typedef struct {
  Vec3 _0;
  Vec3 _1;
  Vec3 _2;
} Mat3;

Vec3 vec3_add(const Vec3 *v1, const Vec3 *v2);
Vec3 *vec3_add_inplace(Vec3 *v1, const Vec3 *v2);
Vec3 vec3_difference(const Vec3 *v1, const Vec3 *v2);
bool vec3_equal(const Vec3 *v1, const Vec3 *v2);
bool vec3_equal_with_error_margin(const Vec3 *v1, const Vec3 *v2, double margin);
double vec3_dot(const Vec3 *v1, const Vec3 *v2);
double vec3_magnitude_squared(const Vec3 *v);
Vec3 vec3_scalar_mul(const Vec3 *v, double scalar);
Vec3 *vec3_scalar_mul_inplace(Vec3 *v, double scalar);
Vec3 vec3_scalar_div(const Vec3 *v, double scalar);
double vec3_magnitude(const Vec3 *v);
Vec3 normalized(const Vec3 *v);
Vec3 vm_mul(Mat3 *mat, Vec3 *v);
Vec3 *vm_mul_inplace(Mat3 *mat, Vec3 *v);
Vec3 *vec3_rotate_by_inplace(Vec3 *v, double angle_rad);
Vec3 project_with_x_rotation(Vec3 *move, Vec3 *to_move, double x_rot, double dist);
Vec3 project_with_y_rotation(Vec3 *move, Vec3 *to_move, double y_rot, double dist);
void move_with_x_rotation(Vec3 *move, Vec3 *to_move, double x_rot, double speed);
void move_with_y_rotation(Vec3 *move, Vec3 *to_move, double y_rot, double speed);
Vec3 rot_x_around_point(Vec3 *point, Vec3 *to_rotate, double angle_rad);
Vec3 rot_y_around_point(Vec3 *point, Vec3 *to_rotate, double angle_rad);

Mat3 rot_x_mat(double angle_rad);
Mat3 rot_y_mat(double angle_rad);
#endif
