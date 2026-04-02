#include "raytracer.h"
#include "utils.h"
#include "vec3.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <time.h>

Object make_metal(Vec3 center, double radius, Vec3 albedo, double fuzz) {
  return (Object){
      .type = SPHERE,
      .pos_center = center,
      .albedo = albedo,
      .material = METAL,
      .radius = radius,
      .fuzz = fuzz,
  };
}

Object make_dielectric(Vec3 center, double radius, double refraction) {
  return (Object){
      .type = SPHERE,
      .pos_center = center,
      .albedo = {1.0, 1.0, 1.0},
      .material = DIELECTRIC,
      .radius = radius,
      .refraction_idx = refraction,
  };
}

Object make_lambertian(Vec3 center, double radius, Vec3 albedo) {
  return (Object){
      .type = SPHERE,
      .pos_center = center,
      .albedo = albedo,
      .material = DIFFUSE,
      .radius = radius,
  };
}

World random_scene() {
  World world;

  Object *spheres = (void *)malloc(sizeof(Object) * 500);
  int sphere_idx = 0;
  spheres[sphere_idx++] = make_lambertian((Vec3){0, -1000, 0}, 1000.0, (Vec3){0.5, 0.5, 0.5});

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      double choose_mat = random_double(0.0, 1.0);
      Vec3 center = {a + 0.9 * random_double(0.0, 1.0), 0.2, b + 0.9 * random_double(0.0, 1.0)};

      Vec3 diff = vec3_difference(&center, &(Vec3){4, 0.2, 0});

      if (vec3_magnitude(&diff) > 0.9) {
        if (choose_mat < 0.8) {
          // diffuse
          Vec3 rand1 = random_vec3(0.0, 1.0);
          Vec3 rand2 = random_vec3(0.0, 1.0);
          Vec3 albedo = vec3_mul(&rand1, &rand2);
          spheres[sphere_idx++] = make_lambertian(center, 0.2, albedo);
        } else if (choose_mat < 0.95) {
          // metal
          Vec3 albedo = random_vec3(0.5, 1.0);
          double fuzz = random_double(0, 0.5);
          spheres[sphere_idx++] = make_metal(center, 0.2, albedo, fuzz);
        } else {
          // glass
          spheres[sphere_idx++] = make_dielectric(center, 0.2, 1.5);
        }
      }
    }
  }

  spheres[sphere_idx++] = make_dielectric((Vec3){0, 1, 0}, 1.0, 1.5);
  spheres[sphere_idx++] = make_lambertian((Vec3){-4, 1, 0}, 1.0, (Vec3){0.4, 0.2, 0.1});
  spheres[sphere_idx++] = make_metal((Vec3){4, 1, 0}, 1.0, (Vec3){0.7, 0.6, 0.5}, 0.0);

  world.objects = spheres;
  world.num_objects = sphere_idx;
  return world;
}

int main(int argc, char **argv) {
  const double aspectRatio = 16.0 / 9.0;
  const int imgWidth = 384 * 6;
  const int imgHeight = (int)(imgWidth / aspectRatio);

  srand(time(NULL));

  InitWindow(imgWidth, imgHeight, "Raytracer");

  World world = random_scene();

  TCamera camera = {
      .position = (Vec3){13.0, 2.0, 3.0}, // Camera position
      .target = (Vec3){0.0, 0.0, 0.0},    // Camera looking at point
      .up = (Vec3){0.0, 1.0, 0.0}         // Camera up vector (rotation towards target)
  };

  const double fovy = 20.0;
  const double theta = fovy / 180.0 * PI;
  const double h = tan(theta / 2);
  const double viewportHeight = 2.0 * h;
  const double viewportWidth = viewportHeight * aspectRatio;

  const int SPEED = 4;
  while (!WindowShouldClose()) {
    float direction_fb = 0;
    if (IsKeyDown(KEY_S))
      direction_fb += 1;
    if (IsKeyDown(KEY_W))
      direction_fb -= 1;

    double theta = atan2(camera.position.z, camera.position.x);
    camera.position.x += cos(theta) * direction_fb * SPEED;
    camera.position.z += sin(theta) * direction_fb * SPEED;

    BeginDrawing();

    trace_rays(viewportWidth, viewportHeight, imgWidth, imgHeight, &camera, &world);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
