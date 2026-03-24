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

Vec3 reflect(const Vec3 *v, const Vec3 *n) {
  double factor = 2 * vec3_dot(v, n);
  Vec3 new_n = vec3_scalar_mul(n, factor);
  return vec3_difference(v, &new_n);
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
        rec.point = vec3_add(&rec.point, &ray->origin);

        rec.normal = get_sphere_normal(object, &rec.point);
        rec.normal = normalized(&rec.normal);
        if (vec3_dot(&ray->direction, &rec.normal) >= 0.0) {
          rec.normal = vec3_scalar_mul(&rec.normal, -1.0);
        }

        rec.object = object;
        rec.didHit = 1;
      }
    } break;
    default:
      break;
    }
  }

  return rec;
}

TRay scatter_lambertian(const HitInfo *hinfo) {
  Vec3 scattered = random_unit_vector();
  scattered = vec3_add(&scattered, &hinfo->normal);
  scattered = vec3_add(&scattered, &hinfo->point);
  return (TRay){hinfo->point, vec3_difference(&scattered, &hinfo->point)};
}

TRay scatter_metal(const HitInfo *hinfo, const TRay *r_in) {
  Vec3 unit_direction = normalized(&r_in->direction);

  Vec3 reflected = reflect(&unit_direction, &hinfo->normal);
  Vec3 fuzzy = random_unit_vector();
  fuzzy = vec3_scalar_mul(&fuzzy, hinfo->object->fuzz);
  vec3_add_inplace(&reflected, &fuzzy);

  TRay scattered = {hinfo->point, reflected};
  return scattered;
}

int scatter(const HitInfo *rec, const TRay *ray, TRay *scattered) {
  if (rec->object->material == DIFFUSE)
    *scattered = scatter_lambertian(rec);
  else {
    *scattered = scatter_metal(rec, ray);
    if (vec3_dot(&scattered->direction, &rec->normal) <= 0.0) {
      return 0;
    }
  }

  return 1;
}

Vec3 ray_color(const TRay *ray, const World *world, int depth) {
  if (depth <= 0) {
    return (Vec3){0.0, 0.0, 0.0};
  }

  HitInfo rec = hit(ray, world, 0.001, INFINITY);

  if (rec.didHit) {
    TRay scattered;
    if (scatter(&rec, ray, &scattered)) {
      Vec3 color = ray_color(&scattered, world, depth - 1);
      return (Vec3){color.x * rec.object->albedo.x, color.y * rec.object->albedo.y, color.z * rec.object->albedo.z};
    }

    return (Vec3){0.0, 0.0, 0.0};
  }

  return computeBgColor(ray);
}

void trace_rays(double viewportWidth, double viewportHeight, int imgWidth, int imgHeight, TCamera *camera,
                World *world) {
  double samplesPerPixel = 64.0;
  Vec3 lowerLeftCorner = {-viewportWidth / 2.0, -viewportHeight / 2.0, 1.0};

  for (int y = 0; y < imgHeight; y++) {
    printf("Scan rows remaining: %d\n", imgHeight - y - 1);

    for (int x = 0; x < imgWidth; x++) {
      double r = 0.0, g = 0.0, b = 0.0;

      for (int s = 0; s < samplesPerPixel; s++) {
        const double u = (random_double(0.0, 1.0) + x) / ((double)imgWidth - 1);
        const double v = (random_double(0.0, 1.0) + y) / ((double)imgHeight - 1);

        TRay ray = {camera->position, (Vec3){lowerLeftCorner.x + u * viewportWidth,
                                             lowerLeftCorner.y + v * viewportHeight, lowerLeftCorner.z}};

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
