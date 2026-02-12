#include "raytracer.h"
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

int min(int a, int b) { return (a < b) ? a : b; }
double absf(double a) { return a < 0 ? -a : a; };

void print_vec3(const Vec3 *v) { printf("x: %f y: %f z: %f\n", v->x, v->y, v->z); }

Vec3 vec3_add(const Vec3 *v1, const Vec3 *v2) {
  Vec3 v = {.x = v1->x + v2->x, .y = v1->y + v2->y, .z = v1->z + v2->z};
  return v;
}

Vec3 vec3_difference(const Vec3 *v1, const Vec3 *v2) {
  Vec3 v = {.x = v1->x - v2->x, .y = v1->y - v2->y, .z = v1->z - v2->z};
  return v;
}

bool vec3_equal(const Vec3 *v1, const Vec3 *v2) { return (v1->x == v2->x) && (v1->y == v2->y) && (v1->z == v2->z); }
bool vec3_equal_with_error_margin(const Vec3 *v1, const Vec3 *v2, double margin) {
  return (absf(v1->x - v2->x) <= margin) && (absf(v1->y - v2->y) <= margin) && (absf(v1->z - v2->z) <= margin);
}

double vec3_dot(const Vec3 *v1, const Vec3 *v2) { return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z; }

double vec3_magnitude_squared(const Vec3 *v) { return vec3_dot(v, v); }

Vec3 vec3_scalar_mul(const Vec3 *v, double scalar) {
  Vec3 res = {.x = v->x * scalar, .y = v->y * scalar, .z = v->z * scalar};
  return res;
}

Vec3 vec3_scalar_div(const Vec3 *v, double scalar) {
  Vec3 res = {.x = v->x / scalar, .y = v->y / scalar, .z = v->z / scalar};
  return res;
}

double vec3_magnitude(const Vec3 *v) { return sqrt(vec3_magnitude_squared(v)); }

Vec3 normalized(const Vec3 *v) { return vec3_scalar_div(v, vec3_magnitude(v)); }

double intersects_with_sphere(const Vec3 *origin, const Vec3 *v, Object *sphere) {
  double mag_sqr_v = vec3_magnitude_squared(v);
  double mag_sqr_c = vec3_magnitude_squared(&sphere->pos_center);
  double dot_cv = vec3_dot(v, &sphere->pos_center);

  double delta = 4 * dot_cv * dot_cv - 4 * mag_sqr_v * (mag_sqr_c - sphere->radius * sphere->radius);
  if (delta < 0)
    return INFINITY;

  double sqrt_delta = sqrt(delta);
  double x1 = (2 * dot_cv - sqrt_delta) / (2 * mag_sqr_v);
  double x2 = (2 * dot_cv + sqrt_delta) / (2 * mag_sqr_v);

  if (x1 > 0 && x1 < x2)
    return x1;
  else if (x2 > 0)
    return x2;
  else
    return INFINITY;
}

Vec3 reflection_of_vector_at_point(const Vec3 *to_reflect, const Vec3 *normal, const Vec3 *point) {
  const double dot_nl = vec3_dot(normal, to_reflect);

  if (dot_nl >= 0)
    return *to_reflect;

  const Vec3 scaled_normal = vec3_scalar_div(normal, absf(dot_nl));
  const Vec3 projected_on_normal = vec3_add(point, &scaled_normal);
  const Vec3 to_proj_point = vec3_add(&scaled_normal, to_reflect);
  const Vec3 reflected_point = vec3_add(&projected_on_normal, &to_proj_point);
  return vec3_difference(&reflected_point, point);
}

double compute_light_intensity_ratio_at_point(const Vec3 *origin, const Vec3 *point, const Object *obj,
                                              const Light *light) {
  const Vec3 normal = vec3_difference(&obj->pos_center, point);
  const Vec3 unit_normal = normalized(&normal);

  // TODO: don't normalize every time
  const Vec3 light_normalized = normalized(&light->direction);
  const Vec3 point_normalized = normalized(point);

  // Direction vector of line from point on object to tip of reflected light vector
  const Vec3 direction_vec_reflection = reflection_of_vector_at_point(&light_normalized, &unit_normal, point);

  const Vec3 unit_reflection_vec = normalized(&direction_vec_reflection);

  return absf(vec3_dot(&unit_reflection_vec, &point_normalized));
}

void trace_rays(int halfScreenWidth, int halfScreenHeight, const Vec3 *origin, World *world) {
  for (int x = -halfScreenWidth; x < halfScreenWidth; x++) {
    for (int y = -halfScreenHeight; y < halfScreenHeight; y++) {
      const Vec3 direction = {x, y, 1};
      const Vec3 v = vec3_difference(&direction, origin);

      double min_lambda = INFINITY;
      Color color = WHITE;
      Object *visible_object = NULL;
      for (int obji = 0; obji < world->num_objects; obji++) {
        Object *object = &world->objects[obji];

        switch (object->type) {
        case SPHERE: {
          double lambda = intersects_with_sphere(origin, &v, object);

          if (lambda < min_lambda) {
            min_lambda = lambda;
            color = object->color;
            visible_object = object;
          }
        } break;
        }
      }

      if (min_lambda < INFINITY) {
        for (int i = 0; i < world->num_lights; i++) {
          Light *light = &world->lights[i];
          double light_intensity_ratio = compute_light_intensity_ratio_at_point(origin, &v, visible_object, light);

          double light_intensity_scaled = light->intensity * light_intensity_ratio;
          color.r = min(255, color.r + light_intensity_scaled);
          color.g = min(255, color.g + light_intensity_scaled);
          color.b = min(255, color.b + light_intensity_scaled);
        }

        DrawPixel(x + halfScreenWidth, y + halfScreenHeight, color);
      }
    }
  }
}
