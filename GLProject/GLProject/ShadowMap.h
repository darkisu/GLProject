#pragma once

#include <vector>

#include <GLEW\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"


class ShadowMap
{
public:
	// result	
	GLuint shadowMapTextureID;
	ShadowMap(unsigned int size);
	void Draw(Shader shader);
	~ShadowMap();
};

