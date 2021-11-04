#version 440 core
out vec4 colour;


uniform float mixValue;

void main()
{
	colour = vec4(mix(vec3(0, 0, 1), vec3(1, 0, 0), mixValue), 1);
}