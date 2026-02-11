#include "raytracer.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

int min(int a, int b) { return (a < b) ? a : b; }

Vec3 vec3_add(const Vec3 *v1, const Vec3 *v2) {
  Vec3 v = {.x = v1->x + v2->x, .y = v1->y + v2->y, .z = v1->z + v2->z};
  return v;
}

Vec3 vec3_difference(const Vec3 *v1, const Vec3 *v2) {
  Vec3 v = {.x = v1->x - v2->x, .y = v1->y - v2->y, .z = v1->z - v2->z};
  return v;
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

double intersects_with_sphere(const Vec3 *v, Object *sphere) {
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

double get_light_intensity_at_point(const Vec3 *point, Object *obj, Light **lights, int num_lights) {
  const Vec3 normal = vec3_difference(&obj->pos_center, point);
  const Vec3 unit_normal = normalized(&normal);

  // NOTE: Vec3 representing point has direction inverse to normal
  // Negate dot product to make things easier later on
  const double dot_pn = -vec3_dot(point, &unit_normal);
  const Vec3 dotted_normal = vec3_scalar_mul(&unit_normal, dot_pn);

  const Vec3 projection_origin_on_normal = vec3_add(point, &dotted_normal);
  // NOTE: Since we look for a light vector through the "reflection" the line from origin to point
  // point across a normal vector, we will find a point on that line
  // twice as far from the origin as the projection of the origin on our normal vector.
  const Vec3 tip_ligth_vec = vec3_scalar_mul(&projection_origin_on_normal, 2);
  const Vec3 light_vec = vec3_difference(&tip_ligth_vec, point);
  const Vec3 light_vec_normalized = normalized(&light_vec);

  double total_intensity = 0.0;
  for (int i = 0; i < num_lights; i++) {
    Light *light = lights[i];
    switch (light->type) {
    case DIRECTIONAL:
      total_intensity += light->intensity;
      break;
    }
  }

  return total_intensity;
}

void trace_rays(int halfScreenWidth, int halfScreenHeight, const Vec3 *origin, World *world) {
  for (int x = -halfScreenWidth; x < halfScreenWidth; x++) {
    for (int y = -halfScreenHeight; y < halfScreenHeight; y++) {
      const Vec3 direction = {.x = x, .y = y, .z = 1};
      const Vec3 v = vec3_difference(&direction, origin);

      double min_lambda = INFINITY;
      Color color = WHITE;
      Object *visible_object = NULL;
      for (int obji = 0; obji < world->num_objects; obji++) {
        Object *object = &world->objects[obji];

        switch (object->type) {
        case SPHERE: {
          double lambda = intersects_with_sphere(&v, object);

          if (lambda < min_lambda) {
            min_lambda = lambda;
            color = object->color;
            visible_object = object;
          }
        } break;
        }
      }

      if (min_lambda < INFINITY) {
        double intensity = get_light_intensity_at_point(&v, visible_object, &world->lights, world->num_lights);

        color.r = min(255, color.r + intensity);
        color.g = min(255, color.g + intensity);
        color.b = min(255, color.b + intensity);
        DrawPixel(x + halfScreenWidth, y + halfScreenHeight, color);
      }
    }
  }
}
