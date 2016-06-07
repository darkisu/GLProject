#pragma once
#include "Scene.h"
class DeferredRenderer
{
public:
	DeferredRenderer(Scene *target,GLuint width, GLuint height);
	~DeferredRenderer();

public:
	GLuint normalTex, diffuseTex, geomatryTex, depthTex, KaTex, KdTex, KsTex;
	void drawP1(Shader shader);
private:
	Scene *TargetScene;
	// Deferred Rendering Data
	GLuint FBO;

	GLuint Width, Height;
	void INIT(GLuint width, GLuint height);

};

