#include "raytracer.h"
#include "camera.h"
#include "utils.h"
#include "vec3.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>

double intersects_with_sphere(Vec3 *camera, const Vec3 *v, const Vec3 *camera_to_sphere_center, const double radius) {
  double mag_sqr_v = vec3_magnitude_squared(v);

  double mag_sqr_c = vec3_magnitude_squared(camera_to_sphere_center);
  double dot_cv = vec3_dot(v, camera_to_sphere_center);

  double delta = 4 * dot_cv * dot_cv - 4 * mag_sqr_v * (mag_sqr_c - radius * radius);
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

double compute_light_intensity_ratio_at_point(const Vec3 *point, const Vec3 *sphere_center, const Light *light) {
  const Vec3 normal = vec3_difference(point, sphere_center);
  const Vec3 unit_normal = normalized(&normal);

  // TODO: don't normalize every time
  const Vec3 unit_light = normalized(&light->direction);
  const Vec3 unit_point = normalized(point);

  // Direction vector of line from point on object to tip of reflected light vector
  const Vec3 direction_vec_reflection = reflection_of_vector_at_point(&unit_light, &unit_normal, point);

  // Reflected vector should have (about) the same length as the unit light vector (= 1)
  assert(absf(vec3_magnitude(&direction_vec_reflection) - vec3_magnitude(&unit_light)) < 0.1);

  const Vec3 unit_reflection_vec = normalized(&direction_vec_reflection);

  // We get a light intensity equal to the "percentage" of light coming our way
  // This is to simulate light scattering in some way
  const double result = vec3_dot(&unit_reflection_vec, &unit_point);

  return result < 0 ? -result : 0;
}

void trace_rays(int halfScreenWidth, int halfScreenHeight, RTCamera *camera, World *world) {
  for (int x = -halfScreenWidth; x < halfScreenWidth; x++) {
    for (int y = -halfScreenHeight; y < halfScreenHeight; y++) {
      Vec3 direction = {camera->global_position.x + x, camera->global_position.y + y,
                        camera->global_position.z + halfScreenWidth};
      Vec3 final_direction =
          rot_y_around_point(&camera->global_position, &direction, camera->global_rotation.y - PI / 2.0);
      const Vec3 v = vec3_difference(&final_direction, &camera->global_position);

      double min_lambda = INFINITY;
      Color color = WHITE;
      Vec3 camera_to_sphere_center = {0, 0, 0};
      for (int obji = 0; obji < world->num_objects; obji++) {
        Object *object = &world->objects[obji];

        switch (object->type) {
        case SPHERE: {
          Vec3 cs = vec3_difference(&object->pos_center, &camera->global_position);
          double lambda = intersects_with_sphere(&camera->global_position, &v, &cs, object->radius);

          if (lambda < min_lambda) {
            min_lambda = lambda;
            color = object->color;
            camera_to_sphere_center = cs;
          }
        } break;
        }
      }

      if (min_lambda < INFINITY) {
        for (int i = 0; i < world->num_lights; i++) {
          Light *light = &world->lights[i];
          Vec3 point = vec3_scalar_mul(&v, min_lambda);
          double light_intensity_ratio =
              compute_light_intensity_ratio_at_point(&point, &camera_to_sphere_center, light);

          double light_intensity_scaled = light->intensity * light_intensity_ratio;
          color.r = min(255, color.r + light_intensity_scaled);
          color.g = min(255, color.g + light_intensity_scaled);
          color.b = min(255, color.b + light_intensity_scaled);
        }

        DrawPixel(x + halfScreenWidth, -y + halfScreenHeight, color);
      }
    }
  }
}
