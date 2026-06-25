#version 430 core

#define LIGHT_NUM_MAX 8

#define LIGHT_POINT 0
#define LIGHT_DIR   1
#define LIGHT_SPOT  2

struct Light {
	int   type;
	vec3  position;
	vec3  direction;

	vec3  ambient;
	vec3  diffuse;
	vec3  specular;
	vec3  attenuation;

	// SpotLight
	float innerAngle;
	float outerAngle;
	float falloff;

	// Shadows
	float bias;
	int pcf;
};

uniform mat4  uView;
uniform int   uRenderMode;

uniform int   uLightNum;
uniform Light uLight[LIGHT_NUM_MAX];

uniform sampler2D uDiffuseTex;
uniform sampler2D uNormalTex;
uniform sampler2D uLightTex;


uniform float     uShininess;
uniform bool      uUseDiffuseTex;
uniform bool      uUseNormalTex;

in vec3 vFragPos;
in vec2 vTexCoord;
in mat3 TBN;

in vec4 vFragPosLightSpace;

out vec4 FragColor;

void main() {
	if (uRenderMode == 1) {
		FragColor = vec4(normalize(TBN[2]) * 0.5 + 0.5, 1.0);
		return;
	}
	if (uRenderMode == 2) {
		FragColor = vec4(normalize(TBN[0]) * 0.5 + 0.5, 1.0);
		return;
	}
	if (uRenderMode == 3) {
		FragColor = vec4(normalize(TBN[1]) * 0.5 + 0.5, 1.0);
		return;
	}

	vec3 baseColor;
	if (uUseDiffuseTex) {
		baseColor = texture(uDiffuseTex, vec2(vTexCoord.x, vTexCoord.y)).rgb;
	} else {
		baseColor = vec3(vTexCoord, 0.0);
	}

	vec3 N;
	if (uUseNormalTex) {
		vec3 normalColor = texture(uNormalTex, vTexCoord).rgb;
		vec3 tangentNormal = normalColor * 2.0 - 1.0;
		mat3 reorthogonalizedTBN = mat3(normalize(TBN[0]), normalize(TBN[1]), normalize(TBN[2]));
		N = normalize(reorthogonalizedTBN * tangentNormal);
	} else {
		N = normalize(TBN[2]);
	}
	vec3 V = normalize(-vFragPos);

	vec3 result = vec3(0.0);
	for (int i = 0; i < uLightNum && i < LIGHT_NUM_MAX; ++i) {
		Light light = uLight[i];

		float attenuation = 1.0;
		float spotlight = 1.0;

		vec3 lightPos = vec3(uView * vec4(light.position, 1.0));
		vec3 lightDir = normalize(mat3(uView) * light.direction);

		switch(light.type) {
			case LIGHT_POINT: {
				float D = length(lightPos - vFragPos);

				float denom = light.attenuation.x + light.attenuation.y * D + light.attenuation.z * D * D;
				attenuation = denom > 0.0 ? 1.0 / denom : 1.0;
				break;
			}
			case LIGHT_DIR: {
				break;
			}
			case LIGHT_SPOT: {
			 vec3 L = lightPos - vFragPos;
			 float D = length(L);
			 L = normalize(L);

			 float denom = light.attenuation.x + light.attenuation.y * D + light.attenuation.z * D * D;
			 attenuation = denom > 0.0 ? 1.0 / denom : 1.0;

			 float theta = dot(L, normalize(-lightDir));
			 float cosInner = cos(light.innerAngle);
			 float cosOuter = cos(light.outerAngle);
			 spotlight = clamp((theta - cosOuter) / (cosInner - cosOuter), 0.0, 1.0);
			 spotlight = pow(spotlight, light.falloff);
			 break;
		 }
		}

		vec3 L = light.type == LIGHT_DIR ? normalize(-lightDir) : normalize(lightPos - vFragPos);
		vec3 H = normalize(L + V); 

		vec3 ambient = light.ambient * baseColor;
		vec3 diffuse = vec3(0.0);
		vec3 specular = vec3(0.0);


		float dotNL = max(dot(N, L), 0.0);
		if (dotNL > 0.0) {
			diffuse = light.diffuse * baseColor * dotNL;
			specular = light.specular * pow(max(dot(N, H), 0.0), uShininess); 
		}

		////// SHADOWS //////
		vec3 projCoords = vFragPosLightSpace.xyz / vFragPosLightSpace.w;
		float shadow = 0.0;
		if (projCoords.z <= 1.0) {
			if (projCoords.x >= 0.0 && projCoords.x <= 1.0 && projCoords.y >= 0.0 && projCoords.y <= 1.0) {
				float closestDepth = texture(uLightTex, projCoords.xy).r; 
				float currentDepth = projCoords.z;
				float bias = light.bias+.001;
				int pcf = light.pcf+3;

				vec2 texelSize = vec2(1.0 / 512.0);
				float samples = 0.0;
				for (int x = -pcf; x <= pcf; ++x) {
					for (int y = -pcf; y <= pcf; ++y) {
						float pcfDepth = texture(uLightTex, projCoords.xy + vec2(x, y) * texelSize).r;
						shadow += (currentDepth - bias > pcfDepth) ? 0.0 : 1.0;
						samples += 1.0;
					}
				}
				shadow = shadow / samples;
			}
		}
		////// SHADOWS //////

		result += ambient + attenuation * (shadow * spotlight * (diffuse + specular));
	}

	FragColor = vec4(result, 1.0);
}
