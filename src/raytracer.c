#include "raytracer.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

int min(int a, int b) { return (a < b) ? a : b; }
double absf(double a) { return a < 0 ? -a : a; };

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

Vec3 normalized(const Vec3 *v) { return vec3_scalar_div(v, vec3_magnitude(v)); }

double intersects_with_sphere(const Vec3 *camera, const Vec3 *v, Object *sphere) {
  double mag_sqr_v = vec3_magnitude_squared(v);
  Vec3 camera_to_sphere_center = vec3_difference(&sphere->pos_center, camera);
  double mag_sqr_c = vec3_magnitude_squared(&camera_to_sphere_center);
  double dot_cv = vec3_dot(v, &camera_to_sphere_center);

  double delta = 4 * dot_cv * dot_cv - 4 * mag_sqr_v * (mag_sqr_c - sphere->radius * sphere->radius);
  if (delta < 0)
    return INFINITY;

  double sqrt_delta = sqrt(delta);
  double l1 = (2 * dot_cv - sqrt_delta) / (2 * mag_sqr_v);
  double l2 = (2 * dot_cv + sqrt_delta) / (2 * mag_sqr_v);

  if (l1 > 0 && l1 < l2)
    return l1;
  else if (l2 > 0)
    return l2;
  else
    return INFINITY;
}

Vec3 reflection_of_vector_at_point(const Vec3 *to_reflect, const Vec3 *normal, const Vec3 *point) {
  const double dot_nl = vec3_dot(normal, to_reflect);

  assert(vec3_magnitude(normal) < 1.01);

  if (dot_nl >= 0)
    return *to_reflect;

  const Vec3 scaled_normal = vec3_scalar_mul(normal, -dot_nl);
  // Place the scaled normal in global coordinates to get the coordinates of its tip
  const Vec3 projected_on_normal = vec3_add(point, &scaled_normal);
  const Vec3 to_proj_point = vec3_add(&scaled_normal, to_reflect);
  // Get symmetrical reflection of base of light vector (if it was in global coordinates)
  // through normal vector
  const Vec3 reflected_point = vec3_add(&projected_on_normal, &to_proj_point);
  const Vec3 reflected_vector = vec3_difference(&reflected_point, point);

  return reflected_vector;
}

double compute_light_intensity_ratio_at_point(const Vec3 *point, const Object *obj, const Light *light) {
  const Vec3 normal = vec3_difference(&obj->pos_center, point);
  const Vec3 unit_normal = normalized(&normal);

  // TODO: don't normalize every time
  const Vec3 unit_light = normalized(&light->direction);
  const Vec3 unit_point = normalized(point);

  // Direction vector of line from point on object to tip of reflected light vector
  const Vec3 direction_vec_reflection = reflection_of_vector_at_point(&unit_light, &unit_normal, point);

  // Reflected vector should have (about) the same length as the unit light vector (= 1)
  assert(absf(vec3_magnitude(&direction_vec_reflection) - vec3_magnitude(&unit_light)) < 0.1);

  const Vec3 unit_reflection_vec = normalized(&direction_vec_reflection);

  // We get a light intensity equal to the "percentage" of light is coming our way
  // This is to simulate light scattering in some way
  const double result = vec3_dot(&unit_reflection_vec, &unit_point);

  return result < 0 ? -result : 0;
}

void trace_rays(int halfScreenWidth, int halfScreenHeight, const Vec3 *camera, World *world) {
  for (int x = -halfScreenWidth; x < halfScreenWidth; x++) {
    for (int y = -halfScreenHeight; y < halfScreenHeight; y++) {
      const Vec3 direction = {x, y, 0};
      const Vec3 v = vec3_difference(&direction, camera);

      double min_lambda = INFINITY;
      Color color = WHITE;
      Object *visible_object = NULL;
      for (int obji = 0; obji < world->num_objects; obji++) {
        Object *object = &world->objects[obji];

        switch (object->type) {
        case SPHERE: {
          double lambda = intersects_with_sphere(camera, &v, object);

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
          Vec3 point = vec3_scalar_mul(&v, min_lambda);
          double light_intensity_ratio = compute_light_intensity_ratio_at_point(&point, visible_object, light);

          double light_intensity_scaled = light->intensity * light_intensity_ratio * light_intensity_ratio *
                                          light_intensity_ratio * light_intensity_ratio;
          color.r = min(255, color.r + light_intensity_scaled);
          color.g = min(255, color.g + light_intensity_scaled);
          color.b = min(255, color.b + light_intensity_scaled);
        }

        DrawPixel(x + halfScreenWidth, -y + halfScreenHeight, color);
      }
    }
  }
}
