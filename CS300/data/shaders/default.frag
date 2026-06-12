#version 430

in vec2 uv;

uniform sampler2D tex;
uniform bool drawTex;

struct Light {
	int  type;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
};

uniform int   uLightNum;
uniform Light uLight[8];

out vec4 FragColor;

void main()
{
	if (drawTex) {
		FragColor = texture(tex, vec2(uv.x, 1 - uv.y));
	} else {
		FragColor = vec4(uv.xy,0,1);
	}
}
