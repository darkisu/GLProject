#pragma once
#include <vector>

#include <GLEW\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Shader.h"


class ReflectiveShadowMap
{
public:
	// result	
	GLuint shadowMapTextureID, shadowMapFBO;
	GLuint VPLPosTextureID;
	ReflectiveShadowMap(unsigned int size);
	void setup(Shader shader, glm::vec3 lightPos);
	~ReflectiveShadowMap();
};

