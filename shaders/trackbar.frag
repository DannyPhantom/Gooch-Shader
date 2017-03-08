#version 430 core

uniform sampler2D tex;
uniform bool textureValid = false;

in vec2 uv;

uniform vec3 barColor;
out vec4 color;

void main() {
	if (!textureValid) {
		color = vec4(barColor, 1.0);
	} else {
		color = texture(tex, uv).rgba;
	}
}
