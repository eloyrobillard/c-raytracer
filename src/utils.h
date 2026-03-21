#ifndef UTILS_H
#define UTILS_H
#include "vec3.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

int min(int a, int b);
double absf(double a);
double clamp(double in, double from, double to);

double random_double(double min, double max);
Vec3 random_unit_vector();
#endif
