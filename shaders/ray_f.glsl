#version 330

uniform vec2 resolution;

struct Ray {
  vec3 origin;
  vec3 direction;
};

void main(void) {
  // shift coords and normalize them so they can be passed as colors
  vec2 p = (gl_FragCoord.xy * 2.0 - resolution) / min(resolution.x, resolution.y);

  Ray ray;
  ray.origin = vec3(0, 0, -10);
  ray.direction = vec3(p.x, p.y, 0.0);

  gl_FragColor = vec4(ray.direction, 1.0);
}
