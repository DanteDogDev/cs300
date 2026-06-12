#version 430

#define LIGHT_NUM_MAX 8

#define LIGHT_POINT 0
#define LIGHT_SPOT  1
#define LIGHT_DIR   2

struct Light {
    int  type;

    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 color;
    vec3 specular;

    vec3 attenuation;
    float innerAngle;
    float outerAngle;
    float falloff;
};

uniform int   uLightNum;
uniform Light uLight[LIGHT_NUM_MAX];

uniform vec3 camera;
uniform float shininess;

uniform sampler2D tex;
uniform bool drawTex;

in vec2 uv;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    vec4 BaseColor;
    if (drawTex) {
        BaseColor = texture(tex, vec2(uv.x, 1.0 - uv.y));
    } else {
        // Fixed: Alpha must be 1.0, otherwise the object is invisible/black!
        BaseColor = vec4(1.0, 1.0, 1.0, 1.0); 
    }

    vec3 n = normalize(Normal);
    vec3 v = normalize(camera - FragPos);

    vec3 final_color = vec3(0.0);

    for (int i = 0; i < uLightNum; ++i) {
        Light light = uLight[i];

        vec3 l = vec3(0.0);
        float d = 0.0;
        float attenuation = 1.0;
        float spotlight = 1.0;

        switch (light.type) {
            case LIGHT_POINT: {
                vec3 lightVec = light.position - FragPos;
                d = length(lightVec);
                l = lightVec / max(d, 0.001); // Optimized safe normalization

                float denom = light.attenuation.x + light.attenuation.y * d + light.attenuation.z * (d * d);
                attenuation = denom > 0.0 ? 1.0 / denom : 1.0;
                break;
            }
            case LIGHT_DIR: {
                l = normalize(-light.direction);
                break;
            }
            case LIGHT_SPOT: {
                 vec3 lightVec = light.position - FragPos;
                 d = length(lightVec);
                 l = lightVec / max(d, 0.001);

                 float denom = light.attenuation.x + light.attenuation.y * d + light.attenuation.z * (d * d);
                 attenuation = denom > 0.0 ? 1.0 / denom : 1.0;

                 float theta = dot(l, normalize(-light.direction));

                 float cosInner = cos(radians(light.innerAngle));
                 float cosOuter = cos(radians(light.outerAngle));

                 spotlight = clamp((theta - cosOuter) / (cosInner - cosOuter), 0.0, 1.0);
                 
                 // Fixed: Protect against uninitialized or zero falloff powers
                 if (light.falloff > 0.0) {
                     spotlight = pow(spotlight, light.falloff);
                 }
                 break;
             }
        }

        vec3 r = reflect(-l, n);

        vec3 ambient = BaseColor.rgb * light.ambient;
        vec3 diffuse = BaseColor.rgb * light.color * max(dot(n, l), 0.0);
        vec3 specular = light.specular * pow(max(dot(r, v), 0.0), max(shininess, 1.0));

        // This loop-coupled ambient structure works perfectly now that alpha isn't zero!
        final_color += ambient + attenuation * (spotlight * (diffuse + specular));
    }

    FragColor = vec4(final_color, BaseColor.a);
}
