#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
uniform float uScale; 
out vec4 chCol;
out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0, 0.0, 1.0, 1.0);
    gl_Position = vec4(inPos.x * uScale, inPos.y * uScale, 0.0, 1.0);
    chCol = inCol;
}
