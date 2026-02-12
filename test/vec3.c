#include "../src/raytracer.h"
#include <assert.h>
#include <stdio.h>

int main() {
  const Vec3 to = {1, -1, 0};
  const Vec3 norm = {0, 1, 0};
  const Vec3 p = {0, 0, 0};
  const Vec3 test = reflection_of_vector_at_point(&to, &norm, &p);
  print_vec3(&test);
  assert(test.x == 1 && test.y == 1 && test.z == 0);

  const Vec3 to2 = {1, -1, 1};
  const Vec3 norm2 = {0, 0, -1};
  const Vec3 p2 = {0, 0, 0};
  const Vec3 test2 = reflection_of_vector_at_point(&to2, &norm2, &p2);
  print_vec3(&test2);
  assert(test2.x == 1 && test2.y == -1 && test2.z == -1);

  const Vec3 to3 = {1, -1, 0};
  const Vec3 norm3 = {0, 0, 1};
  const Vec3 p3 = {0, 0, 0};
  const Vec3 test3 = reflection_of_vector_at_point(&to3, &norm3, &p3);
  print_vec3(&test3);
  assert(test3.x == 1 && test3.y == -1 && test3.z == 0);

  printf("All tests ran well!\n");
}
