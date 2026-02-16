#ifndef VEC3_H
#define VEC3_H
#include <stdbool.h>

typedef struct {
  double x;
  double y;
  double z;
} Vec3;

Vec3 vec3_add(const Vec3 *v1, const Vec3 *v2);
Vec3 vec3_difference(const Vec3 *v1, const Vec3 *v2);
bool vec3_equal(const Vec3 *v1, const Vec3 *v2);
bool vec3_equal_with_error_margin(const Vec3 *v1, const Vec3 *v2, double margin);
double vec3_dot(const Vec3 *v1, const Vec3 *v2);
double vec3_magnitude_squared(const Vec3 *v);
Vec3 vec3_scalar_mul(const Vec3 *v, double scalar);
Vec3 vec3_scalar_div(const Vec3 *v, double scalar);
double vec3_magnitude(const Vec3 *v);
Vec3 normalized(const Vec3 *v);
#endif
