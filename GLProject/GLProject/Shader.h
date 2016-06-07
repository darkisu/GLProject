#pragma once

#include <GLEW\glew.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>



class Shader
{
public:
	GLuint Program;
	Shader(const GLchar * vertexPath, const GLchar * fragmentPath);

	~Shader();
	void Use()
	{
		glUseProgram(this->Program);
	}
};

