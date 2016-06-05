#pragma once
#include <string>
using namespace std;
#include <GLEW\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

static const GLfloat SQUARE_V[] = {
	1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Top Right
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom Left
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f // Top Left 
};

static const GLuint SQUARE_I[] = {
	0, 1, 3,  // First Triangle
	1, 2, 3   // Second Triangle
};


class TextureShower
{
public:
	void show();

	TextureShower(GLuint assignedTex, Shader *pShader);
	~TextureShower();
private:
	void INIT();
	GLuint showtex_VBO, showtex_VAO, showtex_EBO;
	GLuint textureID;
	Shader *shader;
};

