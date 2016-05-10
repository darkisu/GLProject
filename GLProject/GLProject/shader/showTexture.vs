#version 330 core
layout (location = 0) in vec3 position;
//layout (location = 1) in vec2 coord;


out vec3 FragPos;
out vec2 UV;

void main()
{
    FragPos = vec3(vec4(position, 1.0f));
	//UV=coord;
}