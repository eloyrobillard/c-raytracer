#include "camera.h"
#include "raytracer.h"
#include <raylib.h>
#include <raymath.h>

#define RLIGHTS_IMPLEMENTATION
#include <rlights.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;
  const int halfWidth = screenWidth / 2;
  const int halfHeight = screenHeight / 2;

  InitWindow(screenWidth, screenHeight, "Raytracer");
  SetTargetFPS(60);

  RTCamera camera = {.global_position = {0, 0, -halfWidth}, .global_rotation = {0, 0, 0}};
  Object objects[] = {{
                          .type = SPHERE,
                          .pos_center = {.x = 0, .y = 0, .z = 150},
                          .color = RED,
                          .radius = 250,
                      },
                      {
                          .type = SPHERE,
                          .pos_center = {.x = -240, .y = -160, .z = 250},
                          .color = BROWN,
                          .radius = 125,
                      },
                      {
                          .type = SPHERE,
                          .pos_center = {.x = 240, .y = 160, .z = 5},
                          .color = BLUE,
                          .radius = 125,
                      }};

  RTLight lights[] = {{.type = DIRECTIONAL, .direction = {.x = 1, .y = -1, .z = 1}, .intensity = 120.0}};

  World world = {.objects = objects, .num_objects = 3, .lights = lights, .num_lights = 1};

  Resources resources = {.lighting_shader =
                             LoadShader("shaders/lighting_vertex.glsl", "shaders/lighting_fragment.glsl")};

  resources.lighting_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(resources.lighting_shader, "mvp");
  resources.lighting_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(resources.lighting_shader, "viewPos");

  Camera cam3d = {0};
  cam3d.position =
      (Vector3){camera.global_position.x, camera.global_position.y, camera.global_position.z}; // Camera position
  cam3d.target = (Vector3){0.0f, 0.0f, 0.0f}; // Camera looking at point
  cam3d.up = (Vector3){0.0f, 1.0f, 0.0f};     // Camera up vector (rotation towards target)
  cam3d.fovy = 45.0f;                         // Camera field-of-view Y
  cam3d.projection = CAMERA_PERSPECTIVE;

  Matrix *transforms = (Matrix *)calloc(world.num_objects, sizeof(Matrix));
  Mesh *meshes = (Mesh *)calloc(world.num_objects, sizeof(Mesh));
  Material *materials = (Material *)calloc(world.num_objects, sizeof(Material));
  for (int i = 0; i < world.num_objects; i++) {
    Object obj = world.objects[i];

    Matrix translation = MatrixTranslate(obj.pos_center.x, obj.pos_center.y, obj.pos_center.z);
    transforms[i] = translation;

    Mesh mesh = GenMeshSphere(obj.radius, 100, 100);
    meshes[i] = mesh;

    Material mat = LoadMaterialDefault();
    mat.shader = resources.lighting_shader;
    mat.maps[MATERIAL_MAP_DIFFUSE].color = obj.color;
    materials[i] = mat;
  }

  int ambientLoc = GetShaderLocation(resources.lighting_shader, "ambient");
  SetShaderValue(resources.lighting_shader, ambientLoc, (float[4]){0.2f, 0.2f, 0.2f, 1.0f}, SHADER_UNIFORM_VEC4);

  CreateLight(LIGHT_DIRECTIONAL, (Vector3){-1000, -1000, -1000}, Vector3Zero(), RAYWHITE, resources.lighting_shader);

  while (!WindowShouldClose()) {
    UpdateCamera(&cam3d, CAMERA_ORBITAL);

    // Update the light shader with the camera view position
    float cameraPos[3] = {cam3d.position.x, cam3d.position.y, cam3d.position.z};
    SetShaderValue(resources.lighting_shader, resources.lighting_shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos,
                   SHADER_UNIFORM_VEC3);

    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode3D(cam3d);

    for (int i = 0; i < world.num_objects; i++) {
      DrawMesh(meshes[i], materials[i], transforms[i]);
    }

    // tilt_camera(&camera, GetFrameTime());
    // move_camera(&camera, GetFrameTime());

    // trace_rays(halfWidth, halfHeight, &camera, &world);

    EndMode3D();

    DrawFPS(10, 10);

    EndDrawing();
  }

  UnloadShader(resources.lighting_shader);
  free(transforms);
  free(meshes);
  free(materials);

  CloseWindow();

  return 0;
}
