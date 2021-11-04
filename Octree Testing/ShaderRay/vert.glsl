#version 440 core
layout (location = 0) in vec3 aPos;

uniform mat4 models[2];
uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat4 model = models[gl_VertexID];
	gl_Position = projection * view * model * vec4(vec3(0), 1.0);
}