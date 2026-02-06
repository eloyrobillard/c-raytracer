#include <raylib.h>
typedef struct {
  int r;
  int g;
  int b;
  float a;
} RGBA;

typedef enum { SPHERE, SQUARE, PLANE } ObjectType;

typedef struct {
  ObjectType type;
  Vector3 center;  // used by spheres, squares and planes (in which case it is
                   // just any point on the plane)
  int radius;      // used by spheres
  Vector3 normal;  // used by planes and squares
  Vector3 normal2; // user by squares
} Object;

typedef struct {
  Object *world_objects;
} World;

RGBA trace_ray(Vector3 origin, Vector2 screen_pixel, World *world) {}
