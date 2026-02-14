#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "raylib.h"

typedef enum { SPHERE, SQUARE, PLANE } ObjectType;
typedef enum { DIRECTIONAL } LightType;

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

Vec3 vec3_add(const Vec3 *v1, const Vec3 *v2);
Vec3 vec3_difference(const Vec3 *v1, const Vec3 *v2);
bool vec3_equal(const Vec3 *v1, const Vec3 *v2);
bool vec3_equal_with_error_margin(const Vec3 *v1, const Vec3 *v2, double margin);
double vec3_dot(const Vec3 *v1, const Vec3 *v2);
double vec3_magnitude_squared(const Vec3 *v);
Vec3 vec3_scalar_mul(const Vec3 *v, double scalar);
Vec3 vec3_scalar_div(const Vec3 *v, double scalar);
double vec3_magnitude(const Vec3 *v);
Vec3 normalized(const Vec3 *v);

void trace_rays(int, int, const Vec3 *origin, World *world);
double compute_light_intensity_ratio_at_point(const Vec3 *point, const Vec3 *sphere_center, const Light *light);
Vec3 reflection_of_vector_at_point(const Vec3 *to_reflect, const Vec3 *normal, const Vec3 *point);
void print_vec3(const Vec3 *v);
#endif
