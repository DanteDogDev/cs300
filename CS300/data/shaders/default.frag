#version 430
in vec3 color;
out vec4 outputcolor;
void main()
{
	 outputcolor = vec4(color, 1.0f);
}
