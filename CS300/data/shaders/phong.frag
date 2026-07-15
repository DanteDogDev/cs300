#version 430 core

uniform mat4  uView;
uniform int   uRenderMode;


uniform sampler2D uDiffuseTex;

uniform bool      uUseDiffuseTex;

in vec3 vFragPos;
in vec2 vTexCoord;
in mat3 TBN;

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


	FragColor = vec4(baseColor, 1.0);
}
