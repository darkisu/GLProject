#pragma once
#include <vector>

#include <GLEW\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Scene.h"


class ReflectiveShadowMap
{
public:
	GLuint size;
	Scene* targetScene;
	glm::mat4 depthMVP;
	static const glm::mat4 biasMatrix;
public:
	// result	
	GLuint shadowMapTextureID, shadowMapFBO;
	GLuint VPLPosTextureID;
	GLuint vizualizedTextureID;
	GLuint VPLNormalTextureID;
	ReflectiveShadowMap(unsigned int size, Scene* target);
	void draw(Shader shader, glm::vec3 lightPos);
	glm::mat4 getShadowMappingMatrix()
	{
		return biasMatrix * depthMVP;
	}
	~ReflectiveShadowMap();
};

