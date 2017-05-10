#pragma once
#include "ReflectiveShadowMap.h"
#include "Scene.h"
#include "ReflectiveShadowMap.h"
#include <glm\glm.hpp>
#include "TextureShower.h"

class LPV
{
private:
	// scene objects
	Scene *scene;
	ReflectiveShadowMap *RSM;

	// texture IDs
	GLuint *SHTexfront, *SHTexback;
	GLuint VPLalignTex;
	GLfloat LPVSize;

	// parameters of SH
	GLuint resolution;
	GLuint depth;
	GLuint width, height, coeffcount;

	// framebuffer
	GLuint gatherFBO;
	GLuint injecFBO;

	// VAOs & VBOs 
	GLuint injecVAO, injecVBO, injecEBO;// gathering use the same

	// Shaders
	Shader* GatheringShader;
	Shader* InjecShader;
public:
	LPV(Scene *targetScene, ReflectiveShadowMap *targetRSM,GLuint targetRes = 32, GLuint depth = 2);
	~LPV();
	void inject();
	void gather();

};

