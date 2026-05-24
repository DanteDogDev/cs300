#version 430

in vec2 uv;

uniform sampler2D tex;
uniform bool drawTex;

out vec4 FragColor;

void main()
{
	if (drawTex) {
		FragColor = texture(tex, vec2(uv.x, 1 - uv.y));
	} else {
		FragColor = vec4(uv.xy,0,1);
	}
}
