#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 vertUV;

out vec2 uv;

uniform mat4 modelMatrix;

void main() {
	gl_Position = modelMatrix * vec4(position, 0.0, 1.0);
	
	uv = vertUV;
}