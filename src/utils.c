#include "utils.h"
#include <raylib.h>

int min(int a, int b) { return (a < b) ? a : b; }

double absf(double a) { return a < 0 ? -a : a; };

double clamp(double in, double from, double to) {
  assert(from < to);
  if (in > to)
    return to;
  if (in < from)
    return from;
  return in;
}

double random_double(double min, double max) {
  // [min,max) の実数乱数を返す
  return min + (max - min) * rand() / (RAND_MAX + 1.0);
}

Vec3 random_unit_vector() {
  double a = random_double(0.0, 2.0 * PI);
  double z = random_double(-1.0, 1);
  double r = sqrt(1.0 - z * z);
  return (Vec3){r * cos(a), r * sin(a), z};
}
