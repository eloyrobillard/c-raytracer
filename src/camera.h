#ifndef CAMERA_H
#define CAMERA_H
#include "vec3.h"

typedef struct {
  Vec3 global_position;
  Vec3 global_rotation;
} RTCamera;

void move_camera(RTCamera *camera, float delta);
void tilt_camera(RTCamera *camera, float delta);

#endif
