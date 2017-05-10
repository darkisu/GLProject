#include "TextureShower.h"



TextureShower::TextureShower()
{
	showTexture_INIT();
}

TextureShower::TextureShower(GLuint texToShow)
{
	showTexture_INIT();
	setTexture(texToShow);
}


TextureShower::~TextureShower()
{
}

const GLfloat TextureShower::SQUARE_V[30] = {
	1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Top Right
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom Left
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f // Top Left 
};

const GLuint TextureShower::SQUARE_I[6] = {
	0, 1, 3,  // First Triangle
	1, 2, 3   // Second Triangle
};
void TextureShower::showTexture_INIT()
{

	glGenVertexArrays(1, &showtex_VAO);
	glGenBuffers(1, &showtex_VBO);
	glGenBuffers(1, &showtex_EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(showtex_VAO);


	glBindBuffer(GL_ARRAY_BUFFER, showtex_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SQUARE_V), SQUARE_V, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, showtex_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SQUARE_I), SQUARE_I, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}


void TextureShower::showTexture(Shader shader)
{
	shader.Use();
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(showtex_VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1f(glGetUniformLocation(shader.Program, "textureToShow"), 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}