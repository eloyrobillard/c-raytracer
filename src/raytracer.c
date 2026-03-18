#include "raytracer.h"
#include "vec3.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>

double intersects_with_sphere(Vec3 *camera, const Vec3 *v, const Vec3 *camera_to_sphere_center, const double radius) {
  double mag_sqr_v = vec3_magnitude_squared(v);

  double mag_sqr_c = vec3_magnitude_squared(camera_to_sphere_center);

  double b = vec3_dot(v, camera_to_sphere_center);
  double c = mag_sqr_c - radius * radius;

  double delta = b * b - mag_sqr_v * c;
  if (delta < 0)
    return INFINITY;

  double sqrt_delta = sqrt(delta);
  double l1 = (2 * b - sqrt_delta) / (2 * mag_sqr_v);
  double l2 = (2 * b + sqrt_delta) / (2 * mag_sqr_v);

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

Color computeBgColor(int screenHeight, int y) {
  // NOTE: Using fixed point arithmetic.
  // See: https://www.3dgep.com/cpp-fast-track-14-fixed-point/
  int norm_y = (y * 255 / screenHeight);

  Vec3 res = vec3_add(&(Vec3){(255 - norm_y), (255 - norm_y), (255 - norm_y)},
                      &(Vec3){(128 * norm_y) >> 8, (179 * norm_y) >> 8, norm_y});

  return (Color){res.x, res.y, res.z, 255};
}

void trace_rays(int halfScreenWidth, int halfScreenHeight, Camera *camera, World *world) {
  for (int x = -halfScreenWidth; x < halfScreenWidth; x++) {
    for (int y = -halfScreenHeight; y < halfScreenHeight; y++) {
      Vec3 direction = {camera->position.x + x, camera->position.y + y, camera->position.z + halfScreenWidth};
      Vec3 x_rot_direction = rot_x_around_point(&camera->position, &direction, camera->target.x);
      Vec3 y_rot_direction = rot_y_around_point(&camera->position, &x_rot_direction, camera->target.y);
      const Vec3 v = vec3_difference(&y_rot_direction, &camera->position);

      double min_lambda = INFINITY;
      Color color = WHITE;
      for (int obji = 0; obji < world->num_objects; obji++) {
        Object *object = &world->objects[obji];

        switch (object->type) {
        case SPHERE: {
          Vec3 cs = vec3_difference(&object->pos_center, &camera->position);
          double lambda = intersects_with_sphere(&camera->position, &v, &cs, object->radius);

          if (lambda < min_lambda) {
            min_lambda = lambda;
            color = object->color;
          }
        } break;
        }
      }

      if (min_lambda < INFINITY) {
        DrawPixel(x + halfScreenWidth, -y + halfScreenHeight, color);
      } else {
        Color bgColor = computeBgColor(halfScreenHeight * 2, y + halfScreenHeight);
        DrawPixel(x + halfScreenWidth, -y + halfScreenHeight, bgColor);
      }
    }
  }
}
