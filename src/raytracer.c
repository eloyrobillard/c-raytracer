#include "raytracer.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

Vec3 vec3_difference(const Vec3 *v1, const Vec3 *v2) {
  Vec3 v = {.x = v1->x - v2->x, .y = v1->y - v2->y, .z = v1->z - v2->z};
  return v;
}

double vec3_distance_squared(const Vec3 *v1, const Vec3 *v2) {
  const Vec3 diff = vec3_difference(v1, v2);
  return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
}

double vec3_dot(const Vec3 *v1, const Vec3 *v2) { return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z; }

double vec3_magnitude_squared(const Vec3 *v) { return v->x * v->x + v->y * v->y + v->z * v->z; }

bool intersects_with_sphere(const Vec3 *v, Object *sphere) {
  double mag_sqr_v = vec3_magnitude_squared(v);
  double mag_sqr_c = vec3_magnitude_squared(&sphere->pos_center);
  double dot_cv = vec3_dot(v, &sphere->pos_center);

  double delta = 4 * dot_cv * dot_cv - 4 * mag_sqr_v * (mag_sqr_c - sphere->radius * sphere->radius);
  if (delta < 0)
    return false;

  double sqrt_delta = sqrt(delta);
  double x1 = (2 * dot_cv - sqrt_delta) / 2 * mag_sqr_v;
  double x2 = (2 * dot_cv + sqrt_delta) / 2 * mag_sqr_v;

  return x1 > 0 || x2 > 0;
}

void trace_rays(int screenWidth, int screenHeight, const Vec3 *origin, World *world) {
  for (int x = 0; x < screenWidth; x++) {
    for (int y = 0; y < screenHeight; y++) {
      const Vec3 direction = {.x = x, .y = y, .z = 0};
      const Vec3 v = vec3_difference(&direction, origin);

      for (int obji = 0; obji < world->num_objects; obji++) {
        Object *object = &world->objects[obji];

        if (intersects_with_sphere(&v, object))
          break;
        else {
          // printf("%d %d\n", x, y);
          DrawPixel(x, y, object->color);
        }
      }
    }
  }
}
