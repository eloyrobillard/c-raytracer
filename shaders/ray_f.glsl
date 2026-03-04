#version 330

// number of samples used for antialiasing
// prefer to use odd numbers
#define AA_ROWS 7
#define AA_COLS 7

#define TMAX 3000.0f

#define PI 3.14159265359

uniform vec2 resolution;
uniform vec3 ambient;
uniform vec3 viewPos;
uniform float currentTime;

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

struct HitInfo {
  bool hit;
  float t;
  vec3 point;
  vec3 outward_normal;
  vec3 color;
};

HitInfo intersectSphere(Ray R, Sphere S, float tmin, float tmax) {
  HitInfo hinfo;
  hinfo.hit = false;
  hinfo.t = 0.0f;
  hinfo.point = vec3(0);
  hinfo.outward_normal = vec3(0);

  float a = dot(R.direction, R.direction);
  float b = -2 * dot(R.direction, S.position - R.origin);
  float c = dot(R.origin - S.position, R.origin - S.position) - (S.radius * S.radius);
  float d = b * b - 4 * a * c;

  if (d > 0.0) {
    float sqrtd = sqrt(d);
    float t = (-b - sqrtd) / (2 * a);
    if (t <= tmin || tmax <= t) {
      t = (-b + sqrtd) / (2 * a);
      if (t <= tmin || tmax <= t) {
        return hinfo;
      }
    }

    hinfo.hit = true;
    hinfo.t = t;
    hinfo.point = R.origin + t * R.direction;
    vec3 normal = (hinfo.point - S.position) / S.radius;
    // make sure normal has direction opposite to ray
    hinfo.outward_normal = (dot(normal, R.direction) < 0.0f) ? normal : -normal;
  }

  return hinfo;
}

vec3 computeColor(HitInfo intersection, Ray ray) {
  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);

  if (intersection.hit) {
    vec3 normal = intersection.outward_normal;
    for (int li = 0; li < MAX_LIGHTS; li++) {
      Light light = lights[li];
      if (light.enabled == 1) {
        vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);

        diffuse += lightColor * max(dot(-normalize(light.target - light.position), normal), 0.0);

        vec3 reflection = reflect(-normalize(light.position - light.target), normal);
        specular += lightColor * pow(max(dot(reflection, normalize(ray.origin - intersection.point)), 0.0), 32.0);
      }
    }

    intersection.color = 0.5 * (normal + intersection.color + vec3(1));
    return (ambient + diffuse + 2 * specular) * intersection.color;
  } else {
    // sky gradient
    float norm_y = (gl_FragCoord.y / resolution.y);
    return (1 - norm_y) * vec3(1) + norm_y * vec3(0.5, 0.7, 1);
  }
}

HitInfo getIntersectionInfo(Ray ray) {
  HitInfo intersection;
  intersection.hit = false;
  intersection.t = TMAX;

  for (int si = 0; si < 3; si++) {
    HitInfo hinfo = intersectSphere(ray, spheres[si], 0.0f, TMAX);
    if (hinfo.hit && (hinfo.t < intersection.t || !hinfo.hit)) {
      hinfo.color = spheres[si].color;
      intersection = hinfo;
    }
  }

  return intersection;
}

float random(vec2 st) {
  return fract(sin(dot(st.xy,
        vec2(12.9898, 78.233))) *
      43758.5453123 * currentTime);
}

float random_clamped(vec2 st, float min, float max) {
  float rand = random(st);
  return mod(rand, (max - min)) + min;
}

vec3 random_unit_vector(vec2 st) {
  float a = random_clamped(st, 0, 2 * PI);
  float z = random_clamped(st, -1, 1);
  float r = sqrt(1 - z * z);
  return vec3(r * cos(a), r * sin(a), z);
}

void main(void) {
  // shift origin to center of screen (coordinates still grow downward)
  vec2 p = gl_FragCoord.xy * 2 - resolution;
  float theta = viewPos.x == 0.0f ? -sign(viewPos.z) * PI / 2.0f : atan(-viewPos.z, -viewPos.x);

  vec3 color = vec3(0.0);

  Ray ray;
  ray.origin = viewPos;
  vec3 p_rotated = vec3(-sin(theta) * p.x, p.y, cos(theta) * p.x);
  ray.direction = normalize(p_rotated - viewPos);

  HitInfo intersection = getIntersectionInfo(ray);
  color += computeColor(intersection, ray);

  if (intersection.hit) {
    for (int y = -(AA_ROWS / 2); y <= AA_ROWS / 2; y++) {
      for (int x = -(AA_COLS / 2); x <= AA_COLS / 2; x++) {
        vec3 p_rotated = vec3(-sin(theta) * (p.x + x), (p.y + y), cos(theta) * (p.x + x));
        ray.direction = normalize(p_rotated - viewPos);

        intersection = getIntersectionInfo(ray);

        color += computeColor(intersection, ray);
      }
    }
    gl_FragColor = vec4(color / (AA_ROWS * AA_COLS), 1.0f);
  }
  else {
    gl_FragColor = vec4(color, 1.0f);
  }
}
