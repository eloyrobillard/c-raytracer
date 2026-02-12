#include "../src/raytracer.h"
#include <assert.h>
#include <stdio.h>

int main() {
  const Vec3 to = {1, -1, 0};
  const Vec3 norm = {0, 1, 0};
  const Vec3 p = {0, 0, 0};
  const Vec3 test = reflection_of_vector_at_point(&to, &norm, &p);

  assert(test.x == 1 && test.y == 1 && test.z == 0);
  printf("All tests ran well!\n");
}
