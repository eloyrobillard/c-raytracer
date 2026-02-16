#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "Vec3.h"
#include "camera.h"
#include "raylib.h"

typedef enum { SPHERE, SQUARE, PLANE } ObjectType;
typedef enum { DIRECTIONAL } LightType;

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
  LightType type;
  Vec3 direction;
  double intensity;
} Light;

typedef struct {
  Object *objects;
  int num_objects;
  Light *lights;
  int num_lights;
} World;

typedef struct {
  Vec3 direction;
  double intensity;
} DirectedLight;

void trace_rays(int, int, RTCamera *camera, World *world);
double compute_light_intensity_ratio_at_point(const Vec3 *point, const Vec3 *sphere_center, const Light *light);
Vec3 reflection_of_vector_at_point(const Vec3 *to_reflect, const Vec3 *normal, const Vec3 *point);
void print_vec3(const Vec3 *v);
#endif
