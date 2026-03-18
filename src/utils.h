#ifndef UTILS_H
#define UTILS_H
#include "raylib.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

typedef Vector3 Vec3;
int min(int a, int b);
double absf(double a);
double clamp(double in, double from, double to);

double random_double(double min, double max);
Vec3 random_unit_vector();
#endif
