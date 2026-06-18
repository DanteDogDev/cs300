#version 430 core

out vec4 FragColor;

layout(location = 1) uniform vec4 uColor; 

void main() {
    FragColor = uColor;
}
