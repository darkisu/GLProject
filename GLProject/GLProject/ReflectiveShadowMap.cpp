#include "ReflectiveShadowMap.h"



ReflectiveShadowMap::ReflectiveShadowMap(unsigned int size, Scene *target)
{
	targetScene = target;
	this->size = size;
	glActiveTexture(GL_TEXTURE0);

	// Generate framebuffer object for Shadowmap
	glGenFramebuffers(1, &shadowMapFBO);

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

	// Generate Shadowmap Texture
	glGenTextures(1, &shadowMapTextureID);
	glBindTexture(GL_TEXTURE_2D, shadowMapTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// Assign texture to framebuffer depth attachment
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapTextureID, 0);

	// Bind back to default texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate VPL Coordinates Texture
	glGenTextures(1, &VPLPosTextureID);
	glBindTexture(GL_TEXTURE_2D, VPLPosTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Assign texture to color attatchment0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, VPLPosTextureID, 0);

	// Generate VPL Coordinates Texture
	glGenTextures(1, &vizualizedTextureID);
	glBindTexture(GL_TEXTURE_2D, vizualizedTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Assign texture to color attatchment0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, vizualizedTextureID, 0);

	// Bind back to default texture
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum RSMFBOs[] = { GL_NONE,GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(3, RSMFBOs);

	// Bind back to default texture and framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 );
}

void ReflectiveShadowMap::draw(Shader shader,glm::vec3 lightPos)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, size, size);
	glm::mat4 depthProjectionMatrix = glm::perspective(45.0f, 1.0f, 1.0f, glm::distance(lightPos, glm::vec3(0.0, 0.0, 0.0))*2.0f);
	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(-0.5, 0.8, 0.0),   glm::vec3(0.0, 1.0 , 0.0));
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix;

	shader.Use();

	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "VPMatrix"), 1, GL_FALSE, glm::value_ptr(depthMVP));

	targetScene->Draw(shader, "ModelProp");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


ReflectiveShadowMap::~ReflectiveShadowMap()
{
}
