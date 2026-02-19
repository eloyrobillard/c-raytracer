#include <assert.h>

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
