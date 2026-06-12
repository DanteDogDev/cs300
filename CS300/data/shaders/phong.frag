#version 430 core

#define LIGHT_NUM_MAX 8

#define LIGHT_POINT 0
#define LIGHT_DIR   1
#define LIGHT_SPOT  2

struct Light {
    int   type;        // LIGHT_POINT / LIGHT_DIR / LIGHT_SPOT

    vec3  ambient;     // ambient coeff * color
    vec3  diffuse;     // light color
    vec3  specular;    // always white

    vec3  position;
    vec3  direction;

    vec3  attenuation; // c1 c2 c3, point and spot
    float innerAngle;  // radians, spot
    float outerAngle;  // radians, spot
    float falloff;     // spot transition
};

uniform int   uLightNum;
uniform Light uLight[LIGHT_NUM_MAX];

uniform float uShininess;

uniform vec3 uCamPos;

// when texturing is off use the uv as color
uniform sampler2D uDiffuseTex;
uniform bool      uUseTexture;

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

void main() {
    // ambient and diffuse color come from the texture
    vec3 baseColor = uUseTexture
        ? texture(uDiffuseTex, vec2(vTexCoord.x, 1.0 - vTexCoord.y)).rgb
        : vec3(vTexCoord, 0.0);

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCamPos - vFragPos);

    vec3 result = vec3(0.0);

    for (int i = 0; i < uLightNum && i < LIGHT_NUM_MAX; ++i) {
        Light light = uLight[i];

        float attenuation = 1.0;
        float spotlight = 1.0;

        switch(light.type) {
            case LIGHT_POINT: {
                vec3 L = light.position - vFragPos;
                float d = length(L);
                L = normalize(L);
                float denom = light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d;
                attenuation = denom > 0.0 ? 1.0 / denom : 1.0;
                break;
            }
            case LIGHT_DIR: {
                vec3 L = -light.direction;
                break;
            }
            case LIGHT_SPOT: {
                vec3 L = light.position - vFragPos;
                float d = length(L);
                L = normalize(L);
                float denom = light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d;
                attenuation = denom > 0.0 ? 1.0 / denom : 1.0;
                float theta = dot(L, normalize(-light.direction));
                float cosInner = cos(light.innerAngle);
                float cosOuter = cos(light.outerAngle);
                spotlight = clamp((theta - cosOuter) / (cosInner - cosOuter), 0.0, 1.0);
                spotlight = pow(spotlight, light.falloff);
                break;
            }
            default:
                break;
        }

        vec3 L = light.type == LIGHT_DIR ? -light.direction : normalize(light.position - vFragPos);
        vec3 R = reflect(-L, N);

        vec3 ambient  = light.ambient * baseColor;
        vec3 diffuse  = light.diffuse * baseColor * max(dot(N, L), 0);
        vec3 specular = light.specular * pow(max(dot(R, V), 0), uShininess);
        result += ambient + attenuation * spotlight * (diffuse + specular);
    }

    FragColor = vec4(result, 1.0);
}
