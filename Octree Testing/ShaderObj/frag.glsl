#version 440 core
out vec4 colour;


void main()
{
    vec3 color = vec3(0, 1, 0);
	colour = vec4(color, 1);
}