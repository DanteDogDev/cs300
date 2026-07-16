#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat4 uView;

uniform mat4 uLightSpaceMat;

uniform mat4 uNormalMatrix;

out vec3 vFragPos;
out vec2 vTexCoord;
out mat3 TBN;
out vec3 vWorldPos;
out vec3 vWorldNormal;

void main() {
	vTexCoord = aTexCoord;
	vec4 worldPos = uModel * vec4(aPos, 1.0);
	vWorldPos = worldPos.xyz;
	vWorldNormal = normalize(mat3(uNormalMatrix) * aNormal);

	vFragPos = (uView * worldPos).xyz;

	vec3 N = normalize(mat3(transpose(inverse(uView * uModel))) * aNormal);
	vec3 T = normalize(vec3(uView * uModel * vec4(aTangent, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	TBN = mat3(T, B, N);

	gl_Position = uMVP * vec4(aPos, 1.0);
}
