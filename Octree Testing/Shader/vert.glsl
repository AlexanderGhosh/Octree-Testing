#version 440 core
layout(location = 0) in vec3 pos;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;


void main() {
	gl_Position = proj * view * vec4(vec3(pos), 1);
}