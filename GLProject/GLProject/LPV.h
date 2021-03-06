#pragma once
#include "ReflectiveShadowMap.h"
#include "Scene.h"
#include "ReflectiveShadowMap.h"
#include <glm\glm.hpp>
#include "TextureShower.h"

class LPV
{
public:
	// scene objects
	Scene *scene;
	ReflectiveShadowMap *RSM;
	glm::vec4 *SHarray, **RSMSHsarray, **SHfrontarray, *VPLalignarray;
	GLuint position;

	// texture IDs
	GLuint *SHTexfront, *SHTexback;
	GLuint VPLalignTex;
	GLuint *lightInfo3D;
	GLuint *RSMSHs;
	GLfloat LPVSize;

	// parameters of LPV
	GLuint resolution;
	GLuint depth;
	GLuint width, height, coeffcount;
	GLfloat propaWeight;

	// framebuffer
	GLuint gatherFBO;
	GLuint injecFBO;
	GLuint propaFBO;

	// VAOs & VBOs 
	GLuint injecVAO, injecVBO, injecEBO;// gathering use the same

	// Shaders
	Shader* GatheringShader;
	Shader* InjecShader;
	Shader* PropagateShader;
public:
	LPV(Scene *targetScene, ReflectiveShadowMap *targetRSM, GLuint targetRes = 32, GLuint depth = 2, GLfloat weight = 1.8);// / 3.1415926535);
	~LPV();
	void inject(glm::vec3 lightPos);
	void gather();
	void propagate(int iteration);

public:
	// propagate direction array
	static const glm::vec3 propaDir[30];
	static const glm::vec3 reprojDir[30];

};

