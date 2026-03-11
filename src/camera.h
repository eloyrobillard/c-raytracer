#ifndef CAMERA_H
#define CAMERA_H
#include <raylib.h>

#include "vec3.h"

void move_camera(Camera *camera, float delta);
void tilt_camera(Camera *camera, float delta);

#endif
