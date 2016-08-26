#include "ReflectiveShadowMap.h"



ReflectiveShadowMap::ReflectiveShadowMap(unsigned int size)
{
	// Generate framebuffer object for Shadowmap
	glGenFramebuffers(1, &shadowMapFBO);

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

	// Generate Shadowmap Texture
	glGenTextures(1, &shadowMapTextureID);
	glBindTexture(GL_TEXTURE_2D, shadowMapTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Assign texture to framebuffer depth attachment
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapTextureID, 0);

	// Bind back to default texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate VPL Coordinates Texture
	glGenTextures(1, &VPLPosTextureID);
	glBindTexture(GL_TEXTURE_2D, VPLPosTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Assign texture to color attatchment0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, VPLPosTextureID, 0);

	GLenum RSMFBOs[] = { GL_NONE,GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(2, RSMFBOs);

	// Bind back to default texture and framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 );
}

void ReflectiveShadowMap::setup(Shader shader,glm::vec3 lightPos)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	glm::mat4 depthMVP = depthViewMatrix * depthModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "depthProp"), 1, GL_FALSE, glm::value_ptr(depthMVP));
}


ReflectiveShadowMap::~ReflectiveShadowMap()
{
}
