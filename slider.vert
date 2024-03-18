#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
layout(location = 2) in vec3 inCol;
out vec2 chTex;
out vec3 chCol;

uniform vec2 uPos;

void main()
{
	gl_Position = vec4(inPos + uPos, 0.0, 1.0);
	chTex = inTex;
    chCol = inCol;
}