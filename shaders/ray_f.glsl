#version 330

// number of samples used for antialiasing
// prefer to use odd numbers
#define AA_ROWS 10.0f
#define AA_COLS 10.0f

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

#define NUM_SPHERES 3

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
  int sphereIdx;
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

vec3 computeBgColor(float y) {
  // sky gradient
  float norm_y = (y / resolution.y);
  return (1 - norm_y) * vec3(1) + norm_y * vec3(0.5, 0.7, 1);
}

vec3 computeLighting(HitInfo intersection, Ray ray, vec3 color) {
  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);

  vec3 normal = intersection.outward_normal;
  for (int li = 0; li < MAX_LIGHTS; li++) {
    Light light = lights[li];

    if (light.enabled == 1) {
      vec3 lightColor = light.color.xyz;
      vec3 lightVec = normalize(light.target - light.position);

      Ray lightRay;
      lightRay.origin = intersection.point;
      lightRay.direction = lightVec;

      bool inShadow = false;

      for (int i = 0; i < NUM_SPHERES; i++) {
        if (i == intersection.sphereIdx) continue;
        HitInfo hinfo = intersectSphere(lightRay, spheres[i], 0.0f, TMAX);

        if (hinfo.hit) {
          inShadow = true;
        }
      }

      if (!inShadow) {
        diffuse += lightColor * max(dot(-lightVec, normal), 0.0);

        vec3 reflection = reflect(lightVec, normal);
        specular += lightColor * pow(max(dot(reflection, normalize(ray.origin - intersection.point)), 0.0), 32.0);
      }
    }
  }

  return (ambient + diffuse + 2 * specular) * color;
}

vec3 computeColor(HitInfo intersection) {
  return 0.5 * (intersection.outward_normal + intersection.color + vec3(1));
}

HitInfo getIntersectionInfo(Ray ray) {
  HitInfo intersection;
  intersection.hit = false;
  intersection.t = TMAX;

  for (int si = 0; si < NUM_SPHERES; si++) {
    HitInfo hinfo = intersectSphere(ray, spheres[si], 0.0f, TMAX);
    if (hinfo.hit && (hinfo.t < intersection.t || !hinfo.hit)) {
      hinfo.color = spheres[si].color;
      hinfo.sphereIdx = si;
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

vec3 applyAntialiasing(Ray ray, vec2 p, float theta) {
  vec3 color = vec3(0.0f);

  float dy = 1.0f / AA_ROWS;
  float dx = 1.0f / AA_COLS;
  HitInfo rec;
  for (float y = -0.9999f; y < 1.0f; y += 2.0f * dy) {
    for (float x = -0.9999f; x < 1.0f; x += 2.0f * dx) {
      vec3 p_rotated = vec3(-sin(theta) * (p.x + x), (p.y + y), cos(theta) * (p.x + x));
      ray.direction = normalize(p_rotated - viewPos);

      rec = getIntersectionInfo(ray);

      if (rec.hit)
        color += computeColor(rec);
      else
        color += computeBgColor(p_rotated.y);
    }
  }

  return color / (AA_ROWS * AA_COLS);
}

void main(void) {
  // shift origin to center of screen (coordinates still grow downward)
  vec2 p = gl_FragCoord.xy * 2 - resolution;
  float theta = viewPos.x == 0.0f ? -sign(viewPos.z) * PI / 2.0f : atan(-viewPos.z, -viewPos.x);

  Ray ray;
  ray.origin = viewPos;
  vec3 p_rotated = vec3(-sin(theta) * p.x, p.y, cos(theta) * p.x);
  ray.direction = normalize(p_rotated - viewPos);

  HitInfo intersection = getIntersectionInfo(ray);

  vec3 color = applyAntialiasing(ray, p, theta);
  if (intersection.hit) {
    color = computeLighting(intersection, ray, color);
  }

  gl_FragColor = vec4(color, 1.0f);
}
