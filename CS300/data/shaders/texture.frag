#version 330 core

uniform sampler2D uTex;

in vec2 TexCoord;
out vec4 FragColor;

float LinearizeDepth(float depth) 
{
    float near = 1.0;
    float far = 100.0;
    
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float depthValue = texture(uTex, TexCoord).r;
    
    float c = LinearizeDepth(depthValue) / 100.0;
    
    FragColor = vec4(c, c, c, 1.0);
}
