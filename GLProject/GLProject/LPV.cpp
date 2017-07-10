#include "LPV.h"
#include <sstream>
#define SIDEWEIGHT 1.0//0.4234413544f/3.1415926535//3
#define FRONTWEIGHT 1.0//0.4006696846f/3.1415926535//2

const glm::vec3 LPV::propaDir[30] = 
{	
	glm::vec3(-1.0, 0.0, 0.0),				//X+
	glm::vec3(-0.89445, 0.44733, 0.0),
	glm::vec3(-0.89445, -0.44733, 0.0),
	glm::vec3(-0.89445, 0.0, 0.44733),
	glm::vec3(-0.89445, 0.0, -0.44733),
	glm::vec3(1.0, 0.0, 0.0),				//X-
	glm::vec3(0.89445, 0.44733, 0.0),
	glm::vec3(0.89445, -0.44733, 0.0),
	glm::vec3(0.89445, 0.0, 0.44733),
	glm::vec3(0.89445, 0.0, -0.44733),
	glm::vec3(0.0, -1.0, 0.0),				//Y+
	glm::vec3(0.44733, -0.89445, 0.0),
	glm::vec3(-0.44733, -0.89445, 0.0),
	glm::vec3(0.0, -0.89445, 0.44733),
	glm::vec3(0.0, -0.89445, -0.44733),
	glm::vec3(0.0, 1.0, 0.0),				//Y-
	glm::vec3(0.44733, 0.89445, 0.0),
	glm::vec3(-0.44733, 0.89445, 0.0),
	glm::vec3(0.0, 0.89445, 0.44733),
	glm::vec3(0.0, 0.89445, -0.44733),
	glm::vec3(0.0, 0.0, -1.0),				//Z+
	glm::vec3(0.44733, 0.0, -0.89445),
	glm::vec3(-0.44733, 0.0, -0.89445),
	glm::vec3(0.0, 0.44733, -0.89445),
	glm::vec3(0.0, -0.44733, -0.89445),
	glm::vec3(0.0, 0.0, 1.0),				//Z-
	glm::vec3(0.44733, 0.0, 0.89445),
	glm::vec3(-0.44733, 0.0, 0.89445),
	glm::vec3(0.0, 0.44733, 0.89445),
	glm::vec3(0.0, -0.44733, 0.89445)
};
const glm::vec3 LPV::reprojDir[30] = 
{
	glm::vec3(-FRONTWEIGHT, 0.0, 0.0),				//X+
	glm::vec3(0.0, SIDEWEIGHT, 0.0),
	glm::vec3(0.0, -SIDEWEIGHT, 0.0),
	glm::vec3(0.0, 0.0, SIDEWEIGHT),
	glm::vec3(0.0, 0.0, -SIDEWEIGHT),
	glm::vec3(FRONTWEIGHT, 0.0, 0.0),				//X-
	glm::vec3(0.0, SIDEWEIGHT, 0.0),
	glm::vec3(0.0, -SIDEWEIGHT, 0.0),
	glm::vec3(0.0, 0.0, SIDEWEIGHT),
	glm::vec3(0.0, 0.0, -SIDEWEIGHT),
	glm::vec3(0.0, -FRONTWEIGHT, 0.0),				//Y+
	glm::vec3(SIDEWEIGHT, 0.0, 0.0),
	glm::vec3(-SIDEWEIGHT, 0.0, 0.0),
	glm::vec3(0.0, 0.0, SIDEWEIGHT),
	glm::vec3(0.0, 0.0, -SIDEWEIGHT),
	glm::vec3(0.0, FRONTWEIGHT, 0.0),				//Y-
	glm::vec3(SIDEWEIGHT, 0.0, 0.0),
	glm::vec3(-SIDEWEIGHT, 0.0, 0.0),
	glm::vec3(0.0, 0.0, SIDEWEIGHT),
	glm::vec3(0.0, 0.0, -SIDEWEIGHT),
	glm::vec3(0.0, 0.0, -FRONTWEIGHT),				//Z+
	glm::vec3(SIDEWEIGHT, 0.0, 0.0),
	glm::vec3(-SIDEWEIGHT, 0.0, 0.0),
	glm::vec3(0.0, SIDEWEIGHT, 0.0),
	glm::vec3(0.0, -SIDEWEIGHT, 0.0),
	glm::vec3(0.0, 0.0, FRONTWEIGHT),				//Z-
	glm::vec3(SIDEWEIGHT, 0.0, 0.0),
	glm::vec3(-SIDEWEIGHT, 0.0, 0.0),
	glm::vec3(0.0, SIDEWEIGHT, 0.0),
	glm::vec3(0.0, SIDEWEIGHT, 0.0)
};

LPV::LPV(Scene *targetScene, ReflectiveShadowMap *targetRSM, GLuint targetRes , GLuint targetdepth, GLfloat weight)
{
	scene = targetScene;
	RSM = targetRSM;
	resolution = targetRes;
	propaWeight = weight;

	LPVSize = scene->bCubeLength / (GLfloat)resolution;

	depth = targetdepth;
	width = targetRes;
	height = targetRes * targetRes;
	coeffcount = (depth)*(depth - 1) + depth;

	glActiveTexture(GL_TEXTURE0);

	SHTexfront = new GLuint[coeffcount];
	SHTexback = new GLuint[coeffcount];
	lightInfo3D = new GLuint[coeffcount];
	RSMSHs = new GLuint[coeffcount];
	RSMSHsarray = new glm::vec4* [coeffcount];
	
	VPLalignarray = new glm::vec4[std::pow(RSM->size, 2)];
	SHarray = new glm::vec4[std::pow(resolution, 3)];

	// spherical harmonics texture
	for (int i = 0; i < coeffcount; i++)
	{
		// prepare front texture
		glGenTextures(1, &SHTexfront[i]);

		glBindTexture(GL_TEXTURE_2D, SHTexfront[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// prepare back texture
		glGenTextures(1, &SHTexback[i]);

		glBindTexture(GL_TEXTURE_2D, SHTexback[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// light info 3D texture
		glGenTextures(1, &lightInfo3D[i]);
		glBindTexture(GL_TEXTURE_3D, lightInfo3D[i]);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// RSM->SH texture
		glGenTextures(1, &RSMSHs[i]);

		glBindTexture(GL_TEXTURE_2D, RSMSHs[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, RSM->size, RSM->size, 0, GL_RGBA, GL_FLOAT, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// array to temporary store SH results
		RSMSHsarray[i] = new glm::vec4[std::pow(RSM->size, 2)];
	}


	// alignment texture
	glGenTextures(1, &VPLalignTex);

	glBindTexture(GL_TEXTURE_2D, VPLalignTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, RSM->size, RSM->size, 0, GL_RGBA, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);




	// square VAO

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


	// --------------gathering framebuffer------------------
	//

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

	for (int i = 0; i < coeffcount; i++)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i, RSMSHs[i], 0);
		drawbuffers.push_back(GL_COLOR_ATTACHMENT1 + i);
	}

	glDrawBuffers(drawbuffers.size(), drawbuffers.data());

	drawbuffers.clear();

	// --------------injection framebuffer------------------
	//
	// injection frambuffer
	glGenFramebuffers(1, &injecFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, injecFBO);

	// bind depth render buffer
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	// bind injection fbo textures

	for (int i = 0; i < coeffcount; i++)
	{
		drawbuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}

	glDrawBuffers(drawbuffers.size(), drawbuffers.data());

	// --------------propagate framebuffer--------------
	//

	glGenFramebuffers(1, &propaFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, propaFBO);

	// bind attatchment in propagate function

	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	glDrawBuffers(drawbuffers.size(), drawbuffers.data());

	//----------------------------------------------

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Shaders
	GatheringShader = new Shader("./shader/LPVgather.glvs", "./shader/LPVgather.glfs", SHADER_FROM_FILE);
	InjecShader = new Shader("./shader/LPVinjec.glvs", "./shader/LPVinjec.glfs", SHADER_FROM_FILE);
	PropagateShader = new Shader("./shader/LPVPropagate.glvs", "./shader/LPVPropagate.glfs", SHADER_FROM_FILE);
}


LPV::~LPV()
{
}

void LPV::inject(glm::vec3 lightPos)
{
	/*
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

	// Normal Texture ---- Texture 3
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, RSM->VPLNormalTextureID);


	// framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, injecFBO);

	// texture to render
	for (int i = 0; i < coeffcount; i++)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, SHTexfront[i], 0);
	}

	// viewport and clear
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shader and uniform
	InjecShader->Use();
	glUniform1i(glGetUniformLocation(InjecShader->Program, "VPLpos"), 0);
	glUniform1i(glGetUniformLocation(InjecShader->Program, "VPLalign"), 1);
	glUniform1i(glGetUniformLocation(InjecShader->Program, "VPLcolor"), 2);
	glUniform1i(glGetUniformLocation(InjecShader->Program, "VPLnorm"), 3);
	glUniform1i(glGetUniformLocation(InjecShader->Program, "VPLres"), RSM->size);
	glUniform1f(glGetUniformLocation(InjecShader->Program, "LPVsize"), LPVSize);
	glUniform1f(glGetUniformLocation(InjecShader->Program, "LPVres"), this->resolution);
	glUniform3fv(glGetUniformLocation(InjecShader->Program, "cubeLB"), 1, glm::value_ptr(scene->bCubeLower));
	glUniform3fv(glGetUniformLocation(InjecShader->Program, "lightPos"), 1, glm::value_ptr(lightPos));
	//glUniform3fv(glGetUniform)

	//draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	for (int i = 0; i < 4; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/
	SHfrontarray = new glm::vec4*[coeffcount];
	for (int i = 0; i < coeffcount; i++)
	{
		glBindTexture(GL_TEXTURE_2D, RSMSHs[i]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, RSMSHsarray[i]);
		SHfrontarray[i] = new glm::vec4[width*height];
	}
	glBindTexture(GL_TEXTURE_2D, VPLalignTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, VPLalignarray);

	glm::vec4 lightWeight = glm::vec4(( std::pow(this->resolution, 2) / std::pow(RSM->size, 2)));
	for (int i = 0; i < std::pow(RSM->size, 2); i++)
	{
		
		if (VPLalignarray[i].w > 0)
		{
			position = VPLalignarray[i].x + VPLalignarray[i].y * resolution + VPLalignarray[i].z * std::pow(resolution, 2);
			for (int j = 0; j < coeffcount; j++)
			{
				SHfrontarray[j][position] +=  lightWeight * RSMSHsarray[j][i];
				SHfrontarray[j][position].w = 1.0;
			}
		}
	}
	/*
	for (int i = 0; i < coeffcount; i++)
	{
		for (int j = 0; j < width * height; j++)
		{
			SHfrontarray[i][j] *= this->LPVSize / std::pow(RSM->size, 2);
		}
	}*/
	for (int i = 0; i < coeffcount; i++)
	{
		glBindTexture(GL_TEXTURE_2D, SHTexfront[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, SHfrontarray[i]);
		delete[] SHfrontarray[i];
	}
	delete[] SHfrontarray;
}

void LPV::gather()
{

	// Position Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RSM->VPLPosTextureID);

	// Color Texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, RSM->vizualizedTextureID);

	// Normal Texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, RSM->VPLNormalTextureID);

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
	glUniform1i(glGetUniformLocation(GatheringShader->Program, "VPLcolor"), 1);
	glUniform1i(glGetUniformLocation(GatheringShader->Program, "VPLnormal"), 2);
	glUniform1f(glGetUniformLocation(GatheringShader->Program, "LPVsize"), LPVSize);
	glUniform1f(glGetUniformLocation(GatheringShader->Program, "LPVres"), resolution);
	glUniform3fv(glGetUniformLocation(GatheringShader->Program, "lowerbound"), 1, glm::value_ptr( scene->bCubeLower ));

	//Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LPV::propagate(int iteration)
{
	// Vertex Array
	glBindVertexArray(injecVAO);

	// Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, propaFBO);

	for (int j = 0; j < iteration; j++)
	{	// SH Textures after propagation assign color attachment
		for (int i = 0; i < coeffcount; i++)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, SHTexback[i], 0);
		}

		// Viewport and Clear
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// SH Textures befor propagation
		for (int i = 0; i < coeffcount; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, SHTexfront[i]);
		}

		// shader and uniform
		PropagateShader->Use();
		glUniform1i(glGetUniformLocation(PropagateShader->Program, "SH00"), 0);
		glUniform1i(glGetUniformLocation(PropagateShader->Program, "SHn11"), 1);
		glUniform1i(glGetUniformLocation(PropagateShader->Program, "SH01"), 2);
		glUniform1i(glGetUniformLocation(PropagateShader->Program, "SHp11"), 3);
		glUniform1f(glGetUniformLocation(PropagateShader->Program, "LPVres"), resolution);
		glUniform3fv(glGetUniformLocation(PropagateShader->Program, "propaDir"), 30, glm::value_ptr(propaDir[0]));
		glUniform3fv(glGetUniformLocation(PropagateShader->Program, "reprojDir"), 30, glm::value_ptr(reprojDir[0]));
		glUniform1f(glGetUniformLocation(PropagateShader->Program, "propaWeight"), propaWeight / (6.0 * iteration));

		//Draw
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// swap front and back texture
		swap(SHTexfront, SHTexback);
	}

	// push result to 3D texture
	for (int i = 0; i < coeffcount; i++)
	{
		glBindTexture(GL_TEXTURE_3D, lightInfo3D[i]);
		glBindTexture(GL_TEXTURE_2D, SHTexfront[i]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, SHarray);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, resolution, resolution, resolution, 0, GL_RGBA, GL_FLOAT, SHarray);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}
