#version 440 core
out vec4 colour;


uniform float mixValue;

void main()
{
	colour = vec4(mix(vec3(1, 0, 0), vec3(0, 0, 1), mixValue), 1);
}