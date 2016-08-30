#pragma once
#include <vector>

#include <GLEW\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Scene.h"


class ReflectiveShadowMap
{
private:
	GLuint size;
	Scene* targetScene;
public:
	// result	
	GLuint shadowMapTextureID, shadowMapFBO;
	GLuint VPLPosTextureID;
	GLuint vizualizedTextureID;
	ReflectiveShadowMap(unsigned int size, Scene* target);
	void draw(Shader shader, glm::vec3 lightPos);
	~ReflectiveShadowMap();
};

