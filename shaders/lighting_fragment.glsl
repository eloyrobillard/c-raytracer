#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
// uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

#define     MAX_LIGHTS              1
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1

struct Light {
  int enabled;
  int type;
  vec3 position;
  vec3 target;
  vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;
uniform vec3 viewPos;

void main()
{
  vec3 lightDot = vec3(0.0);
  vec3 normal = normalize(fragNormal);
  vec3 viewD = normalize(viewPos - fragPosition);
  vec3 specular = vec3(0.0);

  vec4 tint = colDiffuse * fragColor;

  for (int i = 0; i < MAX_LIGHTS; i++)
  {
    if (lights[i].enabled == 1)
    {
      vec3 light = vec3(0.0);
      vec3 lightColor = lights[i].color.rgb;

      if (lights[i].type == LIGHT_DIRECTIONAL)
      {
        light = -normalize(lights[i].target - lights[i].position);
      }

      float NdotL = max(dot(normal, light), 0.0);
      lightDot += lightColor * NdotL;

      float specCo = 0.0;
      if (NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-(light), normal))), 32.0); // 32 refers to shine
      specular += lightColor * specCo;
    }
  }

  finalColor = (5 * vec4(specular, 1.0) + vec4(lightDot, 1.0) + ambient) * tint;
}
