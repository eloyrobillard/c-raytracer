#version 330

// number of samples used for antialiasing
// prefer to use odd numbers
#define AA_ROWS 10.0f
#define AA_COLS 10.0f

#define TMAX 9000.0f

#define PI 3.14159265359

uniform vec2 resolution;
uniform vec3 ambient;
uniform vec3 viewPos;

struct Ray {
  vec3 origin;
  vec3 direction;
};

#define MATERIAL_DIFFUSE 0
#define MATERIAL_METAL 1
struct Sphere {
  vec3 position;
  float radius;
  vec3 color;
  int material;
};

#define NUM_SPHERES 4
uniform Sphere spheres[NUM_SPHERES];

struct Light {
  int enabled;
  int type;
  vec3 position;
  vec3 target;
  vec4 color;
};

#define MAX_LIGHTS 1
uniform Light lights[MAX_LIGHTS];

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
  float halfB = -dot(R.direction, S.position - R.origin);
  float c = dot(R.origin - S.position, R.origin - S.position) - (S.radius * S.radius);
  float d = halfB * halfB - a * c;

  if (d > 0.0) {
    float sqrtd = sqrt(d);
    float t = (-halfB - sqrtd) / a;
    if (t <= tmin || tmax <= t) {
      t = (-halfB + sqrtd) / a;
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
      lightRay.direction = -lightVec;

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

vec3 computeColor(HitInfo hinfo, Ray inbound_ray) {
  if (spheres[hinfo.sphereIdx].material == MATERIAL_DIFFUSE) {
    return 0.5 * (hinfo.outward_normal + hinfo.color + vec3(1));
  }

  Ray ray;
  ray.origin = hinfo.point;

  vec3 reflected = reflect(normalize(inbound_ray.direction), hinfo.outward_normal);
  ray.direction = reflected;

  HitInfo rec = getIntersectionInfo(ray);
  return rec.hit ? rec.color : computeBgColor(reflected.y);
}

float random(vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
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

vec3 computeLambertianReflectance(HitInfo hinfo, vec3 init_vector) {
  int depth = 50;

  // NOTE: レイのヒットがなくても、ループは少なくとも一回実行されるので
  // factorを２にして、最初のループで１になる
  float factor = 2.0f;

  vec3 unit_vector = random_unit_vector(init_vector.xy);

  HitInfo rec;
  rec.point = hinfo.point;
  rec.outward_normal = hinfo.outward_normal;

  Ray ray;

  do {
    factor *= 0.5f;

    if (dot(unit_vector, rec.outward_normal) <= 0.0) {
      unit_vector = -unit_vector;
    }

    ray.origin = rec.point;
    ray.direction = rec.outward_normal + unit_vector;

    rec = getIntersectionInfo(ray);

    // NOTE: "永遠"にレイがヒットしそうになったら、完全に色褪せたとみなす
    if (--depth <= 0) {
      return vec3(0.0f);
    }

    unit_vector = random_unit_vector(unit_vector.xy);
  } while (rec.hit);

  return computeBgColor(unit_vector.y) * factor;
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
        color += computeColor(rec, ray);
      else
        color += computeBgColor(p_rotated.y);
    }
  }

  return color / (AA_ROWS * AA_COLS);
}

void main(void) {
  // shift origin to center of screen (coordinates still grow downward)
  vec2 p = gl_FragCoord.xy * 2 - resolution;
  float theta = viewPos.x == 0.0f ? -sign(viewPos.z) * PI / 2.0f : atan(-viewPos.z, viewPos.x);

  Ray ray;
  ray.origin = viewPos;
  vec3 p_rotated = vec3(-sin(theta) * p.x, p.y, cos(theta) * p.x);
  ray.direction = normalize(p_rotated - viewPos);

  HitInfo intersection = getIntersectionInfo(ray);

  vec3 color = applyAntialiasing(ray, p, theta);
  if (intersection.hit) {
    color = computeLighting(intersection, ray, color);
    color = computeLambertianReflectance(intersection, ray.direction);
  }

  gl_FragColor = vec4(color, 1.0f);
}
