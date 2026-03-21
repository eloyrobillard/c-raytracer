#include "raytracer.h"
#include "utils.h"
#include "vec3.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

double intersects_with_sphere(const TRay *ray, Object *sphere, double tmin, double tmax) {
  double a = vec3_magnitude_squared(&ray->direction);
  Vec3 camera_to_sphere_center = vec3_difference(&sphere->pos_center, &ray->origin);

  double mag_sqr_c = vec3_magnitude_squared(&camera_to_sphere_center);

  double half_b = -vec3_dot(&ray->direction, &camera_to_sphere_center);
  double c = mag_sqr_c - sphere->radius * sphere->radius;

  double discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return INFINITY;

  double root = sqrt(discriminant);
  double t1 = (-half_b - root) / a;

  if (tmin < t1 && t1 < tmax)
    return t1;

  double t2 = (-half_b + root) / a;
  if (tmin < t2 && t2 < tmax)
    return t2;

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

Vec3 computeBgColor(const TRay *ray) {
  Vec3 unit_direction = normalized(&ray->direction);
  double t = 0.5 * (unit_direction.y + 1.0);
  return (Vec3){(1.0 - t) + t * 0.5, (1.0 - t) + t * 0.7, 1.0};
}

Vec3 get_sphere_normal(const Object *sphere, const Vec3 *point) { return vec3_difference(point, &sphere->pos_center); }

HitInfo hit(const TRay *ray, const World *world, double tmin, double tmax) {
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
        rec.point = vec3_difference(&rec.point, &ray->origin);

        rec.normal = get_sphere_normal(object, &rec.point);
        rec.normal = normalized(&rec.normal);
        if (vec3_dot(&ray->direction, &rec.normal) > 0.0) {
          rec.normal = vec3_scalar_mul(&rec.normal, -1.0);
        }

        rec.didHit = 1;
      }
    } break;
    }
  }

  return rec;
}

Vec3 ray_color(const TRay *ray, const World *world, int depth) {
  if (depth <= 0) {
    return (Vec3){0.0};
  }

  HitInfo rec = hit(ray, world, 0.001, INFINITY);

  if (rec.didHit) {
    Vec3 target = random_unit_vector();
    target = vec3_add(&target, &rec.normal);
    Vec3 color = ray_color(&(TRay){rec.point, target}, world, depth - 1);
    return (Vec3){color.x / 2.0, color.y / 2.0, color.z / 2.0};
  }

  return computeBgColor(ray);
}

void trace_rays(double viewportWidth, double viewportHeight, int imgWidth, int imgHeight, TCamera *camera,
                World *world) {
  double samplesPerPixel = 64.0;
  Vec3 lowerLeftCorner = {-viewportWidth / 2.0, -viewportHeight / 2.0, 0.0};

  for (int x = 0; x < imgWidth; x++) {
    printf("Scan columns remaining: %d\n", imgWidth - x - 1);

    for (int y = 0; y < imgHeight; y++) {
      // Vec3 x_rot_direction = rot_x_around_point(&camera->position, &direction, camera->target.x);
      // Vec3 y_rot_direction = rot_y_around_point(&camera->position, &x_rot_direction, camera->target.y);

      double r = 0.0, g = 0.0, b = 0.0;

      for (int s = 0; s < samplesPerPixel; s++) {
        Vec3 direction = {camera->target.x + (random_double(0.0, 1.0) + x) / ((double)imgWidth - 1),
                          camera->target.y + (random_double(0.0, 1.0) + y) / ((double)imgHeight - 1),
                          camera->target.z - camera->position.z};

        TRay ray = {camera->position, (Vec3){lowerLeftCorner.x + direction.x * viewportWidth,
                                             lowerLeftCorner.y + direction.y * viewportHeight, direction.z}};

        Vec3 color = ray_color(&ray, world, 50);
        r += color.x;
        g += color.y;
        b += color.z;
      }

      // 色の合計をサンプルの数で割り、gamma = 2.0 のガンマ補正を行う
      double scale = 1.0 / samplesPerPixel;
      r = sqrt(scale * r);
      g = sqrt(scale * g);
      b = sqrt(scale * b);

      DrawPixel(x, -y + imgHeight,
                (Color){256.0 * clamp(r, 0.0, 0.999), 256.0 * clamp(g, 0.0, 0.999), 256.0 * clamp(b, 0.0, 0.999), 255});
    }
  }

  printf("Done!\n");
}
