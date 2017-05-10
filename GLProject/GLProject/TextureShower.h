#pragma once
#include <GLEW\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
class TextureShower
{
public:
	void showTexture(Shader shader);
	void setTexture(GLuint texToShow)
	{
		textureID = texToShow;
	}

public:
	TextureShower();
	TextureShower(GLuint texToShow);
	~TextureShower();

	friend class LPV;

private:
	void showTexture_INIT();
	GLuint showtex_VBO, showtex_VAO, showtex_EBO;
	static const GLfloat SQUARE_V[30];
	static const GLuint SQUARE_I[6];
	GLuint textureID;
};

