#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "raylib.h"

typedef enum { SPHERE, SQUARE, PLANE } ObjectType;

typedef struct {
  double x;
  double y;
  double z;
} Vec3;

typedef struct {
  ObjectType type;
  Vec3 pos_center; // used by spheres, squares and planes (in which case it
                   // is just any point on the plane)
  int radius;      // used by spheres
  Vec3 normal;     // used by planes and squares
  Vec3 normal2;    // used by squares
  Color color;
} Object;

typedef struct {
  Object *objects;
  int num_objects;
} World;

void trace_rays(int screenWidth, int screenHeight, const Vec3 *origin, World *world);
#endif
