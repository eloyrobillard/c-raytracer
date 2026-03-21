#include "raytracer.h"
#include "utils.h"
#include "vec3.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

double intersects_with_sphere(const Ray *ray, Object *sphere, double tmin, double tmax) {
  double mag_sqr_v = vec3_magnitude_squared(&ray->direction);
  Vec3 camera_to_sphere_center = vec3_difference(&sphere->pos_center, &ray->position);

  double mag_sqr_c = vec3_magnitude_squared(&camera_to_sphere_center);

  double b = vec3_dot(&ray->direction, &camera_to_sphere_center);
  double c = mag_sqr_c - sphere->radius * sphere->radius;

  double delta = b * b - mag_sqr_v * c;
  if (delta < 0)
    return INFINITY;

  double sqrt_delta = sqrt(delta);
  double t1 = (2 * b - sqrt_delta) / (2 * mag_sqr_v);
  double t2 = (2 * b + sqrt_delta) / (2 * mag_sqr_v);

  if (t1 > tmin && t1 < tmax)
    return t1;
  else if (t2 > tmin && t2 < tmax)
    return t2;
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

Color computeBgColor(const Ray *ray, int y) {
  Vec3 unit_direction = normalized(&ray->direction);

  // NOTE: Using fixed point arithmetic.
  // See: https://www.3dgep.com/cpp-fast-track-14-fixed-point/
  int t = (int)(128 * (unit_direction.y + 255)) >> 8;

  Vec3 res = vec3_add(&(Vec3){(255 - t), (255 - t), (255 - t)}, &(Vec3){(128 * t) >> 8, (179 * t) >> 8, t});

  return (Color){res.x, res.y, res.z, 255};
}

Vec3 get_sphere_normal(const Object *sphere, const Vec3 *point) { return vec3_difference(point, &sphere->pos_center); }

HitInfo hit(const Ray *ray, const World *world, double tmin, double tmax) {
  HitInfo rec;
  rec.didHit = 0;
  double min_lambda = INFINITY;

  for (int obji = 0; obji < world->num_objects; obji++) {
    Object *object = &world->objects[obji];

    switch (object->type) {
    case SPHERE: {
      double lambda = intersects_with_sphere(ray, object, tmin, tmax);

      if (lambda < min_lambda) {
        min_lambda = lambda;
        rec.point = vec3_scalar_mul(&ray->direction, lambda);
        rec.point = vec3_difference(&rec.point, &ray->position);
        rec.normal = get_sphere_normal(object, &rec.point);
        rec.normal = normalized(&rec.normal);
        rec.didHit = 1;
      }
    } break;
    }
  }

  return rec;
}

Color ray_color(const Ray *ray, const World *world, int depth) {
  if (depth <= 0) {
    return (Color){0};
  }

  HitInfo rec = hit(ray, world, 0.001, INFINITY);

  if (rec.didHit) {
    Vec3 target = random_unit_vector();
    target = vec3_add(&target, &rec.normal);
    Color color = ray_color(&(Ray){rec.point, target}, world, depth - 1);
    return (Color){color.r >> 1, color.g >> 1, color.b >> 1, color.a};
  }

  return computeBgColor(ray, ray->direction.y);
}

void trace_rays(double viewportWidth, double viewportHeight, int imgWidth, int imgHeight, Camera *camera,
                World *world) {
  Vec3 lowerLeftCorner = {-viewportWidth / 2.0, -viewportHeight / 2.0, 0.0};
  for (int x = 0; x < imgWidth; x++) {
    printf("Scan columns remaining: %d\n", imgWidth - x - 1);

    for (int y = 0; y < imgHeight; y++) {
      Vec3 direction = {camera->target.x + x / ((double)imgWidth - 1), camera->target.y + y / ((double)imgHeight - 1),
                        camera->target.z - camera->position.z};
      // Vec3 x_rot_direction = rot_x_around_point(&camera->position, &direction, camera->target.x);
      // Vec3 y_rot_direction = rot_y_around_point(&camera->position, &x_rot_direction, camera->target.y);

      Ray ray = {camera->position, (Vec3){lowerLeftCorner.x + direction.x * viewportWidth,
                                          lowerLeftCorner.y + direction.y * viewportHeight, direction.z}};

      Color color = ray_color(&ray, world, 50);

      DrawPixel(x, -y + imgHeight, color);
    }
  }

  printf("Done!\n");
}
