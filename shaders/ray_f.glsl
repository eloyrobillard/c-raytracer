#version 330

uniform vec2 resolution;

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct Sphere {
  vec3 position;
  float radius;
  vec3 color;
};

uniform Sphere spheres[3];

bool intersectSphere(Ray R, Sphere S) {
  vec3 a = R.origin - S.position;
  float b = dot(a, R.direction);
  float c = dot(a, a) - (S.radius * S.radius);
  float d = b * b - c;

  if (d > 0.0) {
    float t = -b - sqrt(d);
    return (t > 0.0);
  }

  return false;
}

void main(void) {
  // shift origin to center of screen (coordinates still grow downward)
  // normalize coordinates so they can be passed as colors
  vec2 p = (gl_FragCoord.xy * 2.0 - resolution) / min(resolution.x, resolution.y);

  Ray ray;
  ray.origin = vec3(0, 0, -10.0);
  ray.direction = normalize(vec3(p.x, p.y, 1.0));

  vec3 destColor = vec3(0.0);

  for (int si = 0; si < 3; si++) {
    if (intersectSphere(ray, spheres[si])) {
      destColor = spheres[si].color;
    }
  }

  gl_FragColor = vec4(destColor, 1.0);
}
