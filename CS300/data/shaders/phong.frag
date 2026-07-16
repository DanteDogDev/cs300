#version 430 core

uniform mat4  uView;
uniform int   uRenderMode;


uniform sampler2D uDiffuseTex;
uniform samplerCube uEnvCubeMap;

uniform bool  uIsReflector;
uniform int   uReflectorMode;
uniform float uIOR;
uniform vec3  uCamPos;

in vec3 vFragPos;
in vec2 vTexCoord;
in mat3 TBN;

in vec3 vWorldPos;
in vec3 vWorldNormal;

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

	if (uIsReflector) {
		vec3 I = normalize(vWorldPos - uCamPos);
		vec3 N = normalize(vWorldNormal);
		if (uReflectorMode == 1) {
			vec3 R = reflect(I, N);
			FragColor = texture(uEnvCubeMap, R);
			return;
		} else if (uReflectorMode == 2) {
			float ratio = 1.0 / uIOR;
			vec3 R = refract(I, N, ratio);
			FragColor = texture(uEnvCubeMap, R);
			return;
		}
	}

	vec3 baseColor;
	baseColor = texture(uDiffuseTex, vec2(vTexCoord.x, vTexCoord.y)).rgb;


	FragColor = vec4(baseColor, 1.0);
}
