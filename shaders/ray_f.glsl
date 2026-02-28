#version 330

uniform vec2 resolution;
uniform vec3 ambient;

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct Sphere {
  vec3 position;
  float radius;
  vec3 color;
};

struct Light {
  int enabled;
  int type;
  vec3 position;
  vec3 target;
  vec4 color;
};

#define MAX_LIGHTS 1

uniform Light lights[MAX_LIGHTS];

uniform Sphere spheres[3];

float intersectSphere(Ray R, Sphere S) {
  float a = dot(R.direction, R.direction);
  float b = 2 * dot(R.direction, R.origin - S.position);
  float c = dot(R.origin - S.position, R.origin - S.position) - (S.radius * S.radius);
  float d = b * b - 4 * a * c;

  if (d > 0.0) {
    float t = (-b - sqrt(d)) / (2 * a);
    if (t > 0.0)
      return t;
  }

  return 0.0;
}

void main(void) {
  // shift origin to center of screen (coordinates still grow downward)
  // normalize coordinates so that ???
  vec2 p = (gl_FragCoord.xy * 2.0 - resolution) / min(resolution.x, resolution.y);

  Ray ray;
  ray.origin = vec3(0, 0, -960.0);
  ray.direction = normalize(vec3(p.x, p.y, 1.0));

  vec3 destColor = vec3(0.0);
  float closestIntersection = -1;
  vec3 intersectionPoint = vec3(0.0);
  int intersectedSphere = -1;

  for (int si = 0; si < 3; si++) {
    float t = intersectSphere(ray, spheres[si]);
    if (t > 0.0 && (t < closestIntersection || closestIntersection == -1)) {
      closestIntersection = t;
      destColor = spheres[si].color;
      intersectionPoint = ray.origin + ray.direction * t;
      intersectedSphere = si;
    }
  }

  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);

  if (closestIntersection != -1) {
    vec3 normal = (intersectionPoint - spheres[intersectedSphere].position) / spheres[intersectedSphere].radius;
    for (int li = 0; li < MAX_LIGHTS; li++) {
      Light light = lights[li];
      if (light.enabled == 1) {
        vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]) * light.color[3];

        diffuse += lightColor * max(dot(-normalize(light.target - light.position), normal), 0.0);

        vec3 reflection = reflect(-normalize(light.position - light.target), normal);
        specular += lightColor * pow(max(dot(reflection, normalize(ray.origin - intersectionPoint)), 0.0), 32.0);
      }
    }
  }

  gl_FragColor = vec4((ambient + diffuse + specular) * destColor, 1.0);
}
