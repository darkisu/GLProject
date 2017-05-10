#include "LPV.h"
#include <sstream>



LPV::LPV(Scene *targetScene, ReflectiveShadowMap *targetRSM, GLuint targetRes , GLuint targetdepth)
{
	scene = targetScene;
	RSM = targetRSM;
	resolution = targetRes;

	LPVSize = scene->bCubeLength / (GLfloat)resolution;

	depth = targetdepth;
	width = targetRes;
	height = targetRes * targetRes;
	coeffcount = 1 + 2 * (depth - 1);

	glActiveTexture(GL_TEXTURE0);

	SHTexfront = new GLuint[coeffcount];
	SHTexback = new GLuint[coeffcount];


	// spherical harmonics texture
	for (int i = 0; i < coeffcount; i++)
	{
		glGenTextures(1, &SHTexfront[i]);

		glBindTexture(GL_TEXTURE_2D, SHTexfront[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, SHTexfront[i], 0);

	}


	// alignment texture
	glGenTextures(1, &VPLalignTex);

	glBindTexture(GL_TEXTURE_2D, VPLalignTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, RSM->size, RSM->size, 0, GL_RGBA, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, VPLalignTex, 0);


	// --------------gathering setup------------------
	//

	// gatherinig frambuffer
	glGenFramebuffers(1, &gatherFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gatherFBO);

	GLuint depthRenderBuffer;
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RSM->size, RSM->size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
	vector<GLuint> drawbuffers;

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, VPLalignTex, 0);
	drawbuffers.push_back(GL_COLOR_ATTACHMENT0);

	glDrawBuffers(drawbuffers.size(), drawbuffers.data());

	drawbuffers.clear();

	// --------------injection setup------------------
	//

	// injection frambuffer
	glGenFramebuffers(1, &injecFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, injecFBO);


	// VAOs VBOs for injection
	glGenVertexArrays(1, &injecVAO);
	glGenBuffers(1, &injecVBO);
	glGenBuffers(1, &injecEBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(injecVAO);

	glBindBuffer(GL_ARRAY_BUFFER, injecVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TextureShower::SQUARE_V), TextureShower::SQUARE_V, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, injecEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TextureShower::SQUARE_I), TextureShower::SQUARE_I, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);	//vertex in square v

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);	//normals in square v

	glBindVertexArray(0);

	// bind depth render buffer
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	// bind injection fbo textures

	for (int i = 0; i < coeffcount; i++)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, SHTexfront[i], 0);
		drawbuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glDrawBuffers(drawbuffers.size(), drawbuffers.data());
	//----------------------------------------------

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Shaders
	GatheringShader = new Shader("./shader/LPVgather.glvs", "./shader/LPVgather.glfs", SHADER_FROM_FILE);
	InjecShader = new Shader("./shader/LPVinjec.glvs", "./shader/LPVinjec.glfs", SHADER_FROM_FILE);
}


LPV::~LPV()
{
}

void LPV::inject()
{

	// VAO
	glBindVertexArray(injecVAO);

	// Position Texture ---- Texture 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RSM->VPLPosTextureID);

	// Alignment Texture ---- Texture 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, VPLalignTex);

	// Color Texture ---- Texture 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, RSM->vizualizedTextureID);

	// framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, injecFBO);

	// viewport and clear
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shader and uniform
	
}

void LPV::gather()
{

	// Position Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RSM->VPLPosTextureID);

	glBindVertexArray(injecVAO);

	//bind frambuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gatherFBO);
	
	//viewport and clear
	glViewport(0, 0, RSM->size, RSM->size);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Shader and uniform
	GatheringShader->Use();
	glUniform1i(glGetUniformLocation(GatheringShader->Program, "VPLs"), 0);
	glUniform1f(glGetUniformLocation(GatheringShader->Program, "LPVsize"), LPVSize);
	glUniform1f(glGetUniformLocation(GatheringShader->Program, "LPVres"), resolution);
	glUniform3fv(glGetUniformLocation(GatheringShader->Program, "lowerbound"), 1, glm::value_ptr( scene->bCubeOrigin ));

	//Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
