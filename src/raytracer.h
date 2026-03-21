#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "raylib.h"
#include "vec3.h"

typedef enum { SPHERE, SQUARE, PLANE } ObjectType;
typedef enum { DIFFUSE = 0, METAL = 1 } ObjectMaterial;

typedef struct {
  ObjectType type;
  Vec3 pos_center; // used by spheres, squares and planes (in which case it
                   // is just any point on the plane)
  float radius;    // used by spheres
  Vec3 normal;     // used by planes and squares
  Vec3 normal2;    // used by squares
  ObjectMaterial material;
  Color color;
} Object;
//
// Light type
typedef enum { LIGHT_DIRECTIONAL = 0, LIGHT_POINT } LightType;

typedef struct {
  int type;
  bool enabled;
  Vector3 position;
  Vector3 target;
  Color color;
  float attenuation;
} Light;

typedef struct {
  Object *objects;
  int num_objects;
  Light *lights;
  int num_lights;
} World;

typedef struct {
  Vec3 point;
  Vec3 normal;
  int didHit;
} HitInfo;

void trace_rays(double viewportWidth, double viewportHeight, int imgWidth, int imgHeight, Camera *camera, World *world);
Vec3 reflection_of_vector_at_point(const Vec3 *to_reflect, const Vec3 *normal, const Vec3 *point);
#endif
