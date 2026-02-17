#include "vec3.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

void print_vec3(const Vec3 *v) { printf("x: %f y: %f z: %f\n", v->x, v->y, v->z); }

Vec3 vec3_add(const Vec3 *v1, const Vec3 *v2) {
  Vec3 v = {v1->x + v2->x, v1->y + v2->y, v1->z + v2->z};
  return v;
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

Vec3 vec3_scalar_div(const Vec3 *v, double scalar) {
  Vec3 res = {v->x / scalar, v->y / scalar, v->z / scalar};
  return res;
}

double vec3_magnitude(const Vec3 *v) { return sqrt(vec3_magnitude_squared(v)); }
Vec3 normalized(const Vec3 *v) {
  double mag = vec3_magnitude(v);
  return mag ? vec3_scalar_div(v, mag) : *v;
}
