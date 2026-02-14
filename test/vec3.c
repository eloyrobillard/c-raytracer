#include "../src/raytracer.h"
#include <assert.h>
#include <stdio.h>

void test_vector_reflection() {
  const Vec3 to = {1, -1, 0};
  const Vec3 norm = {0, 1, 0};
  const Vec3 p = {0, 0, 0};
  const Vec3 test = reflection_of_vector_at_point(&to, &norm, &p);
  assert(test.x == 1 && test.y == 1 && test.z == 0);

  const Vec3 to2 = {1, -1, 1};
  const Vec3 norm2 = {0, 0, -1};
  const Vec3 p2 = {0, 0, 0};
  const Vec3 test2 = reflection_of_vector_at_point(&to2, &norm2, &p2);
  assert(test2.x == 1 && test2.y == -1 && test2.z == -1);

  const Vec3 to3 = {1, -1, 0};
  const Vec3 norm3 = {0, 0, 1};
  const Vec3 p3 = {0, 0, 0};
  const Vec3 test3 = reflection_of_vector_at_point(&to3, &norm3, &p3);
  assert(test3.x == 1 && test3.y == -1 && test3.z == 0);

  const Vec3 point = {0, 0, 1};
  const Object obj = {SPHERE, {0, 0, 11}, 10};
  const Light light = {DIRECTIONAL, {0, 0, 30}, 100};
  const Vec3 normal = vec3_difference(&point, &obj.pos_center);
  const Vec3 normal_unit = normalized(&normal);
  const Vec3 test4 = reflection_of_vector_at_point(&light.direction, &normal_unit, &point);
  assert(test4.x == 0 && test4.y == 0 && test4.z == -30);
}

void test_light_intensity_ratio_computation() {
  const Vec3 point = {0, 0, 1};
  const Object obj = {SPHERE, {0, 0, 31}, 30};
  const Light light = {DIRECTIONAL, {0, 0, 1}, 100};
  double intensity = compute_light_intensity_ratio_at_point(&point, &obj, &light);
  assert(intensity == 1);
}

int main() {
  test_vector_reflection();
  test_light_intensity_ratio_computation();

  printf("All tests ran well!\n");
}
