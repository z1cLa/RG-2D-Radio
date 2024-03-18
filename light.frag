#version 330 core

out vec4 outCol;

uniform float uActive;

void main()
{
    outCol = vec4(0.8 + uActive / 4, 0.0, 0.0 , 1.0);
}