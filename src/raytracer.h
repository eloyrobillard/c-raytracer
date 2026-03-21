#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "raylib.h"
#include "vec3.h"

typedef enum { SPHERE, SQUARE, PLANE } ObjectType;
typedef enum { DIFFUSE = 0, METAL = 1 } ObjectMaterial;

typedef struct {
  ObjectType type;
  ObjectMaterial material;
  Vec3 pos_center; // used by spheres, squares and planes (in which case it
                   // is just any point on the plane)
  double radius;   // used by spheres
  Vec3 normal;     // used by planes and squares
  Vec3 normal2;    // used by squares
  double fuzz;     // used for a blurry metal effect
  Vec3 albedo;
} Object;

typedef struct {
  Object *objects;
  int num_objects;
} World;

typedef struct {
  Vec3 point;
  Vec3 normal;
  Object *object;
  int didHit;
} HitInfo;

typedef struct {
  Vec3 position;
  Vec3 target;
  Vec3 up;
} TCamera;

void trace_rays(double viewportWidth, double viewportHeight, int imgWidth, int imgHeight, TCamera *camera,
                World *world);
#endif
