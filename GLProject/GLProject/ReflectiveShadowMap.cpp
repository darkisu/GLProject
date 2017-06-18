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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// Assign texture to framebuffer depth attachment
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapTextureID, 0);

	// Bind back to default texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate VPL Coordinates Texture
	glGenTextures(1, &VPLPosTextureID);
	glBindTexture(GL_TEXTURE_2D, VPLPosTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Assign texture to color attatchment0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, VPLPosTextureID, 0);

	// Generate VPL Color Texture
	glGenTextures(1, &vizualizedTextureID);
	glBindTexture(GL_TEXTURE_2D, vizualizedTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);\

	// Assign texture to color attatchment1
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, vizualizedTextureID, 0);


	// Generate VPL Normal Texture
	glGenTextures(1, &VPLNormalTextureID);
	glBindTexture(GL_TEXTURE_2D, VPLNormalTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, VPLNormalTextureID, 0);

	// Bind back to default texture
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum RSMFBOs[] = { GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(4, RSMFBOs);

	// Bind back to default texture and framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 );
}

void ReflectiveShadowMap::draw(Shader shader,glm::vec3 lightPos)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, size, size);
	glm::mat4 depthProjectionMatrix = glm::perspective(45.0f, 1.0f, 1.0f, glm::distance(lightPos, glm::vec3(0.0, 0.0, 0.0))*2.0f);
	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0.0, 0.0, 0.0),   glm::vec3(0.0, 1.0 , 0.0));
	depthMVP = depthProjectionMatrix * depthViewMatrix;

	shader.Use();

	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "VPMatrix"), 1, GL_FALSE, glm::value_ptr(depthMVP));
	glUniform3fv(glGetUniformLocation(shader.Program, "lightpos"), 1, glm::value_ptr(lightPos));

	targetScene->Draw(shader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


ReflectiveShadowMap::~ReflectiveShadowMap()
{
}
const glm::mat4 ReflectiveShadowMap::biasMatrix =
	glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
	glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
	glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
	glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));