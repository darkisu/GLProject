
#include "Scene.h"

class DeferredRenderer
{
private:
	// Deferred Rendering Data
	GLuint defFBO;
	GLuint defNormTex, defDiffTex, defGeomTex, defDepthTex, defKaTex, defKdTex, defKsTex;
	GLuint defWidth, defHeight;
	GLuint defResultTex, defResultFBO;
	Shader *pShaderP1, *pShaderP2;

public:
	DeferredRenderer(
		Scene *target,
		Shader *shaderP1,
		Shader *shaderP2,
		GLuint width,
		GLuint height);
	void step1Draw();
	~DeferredRenderer();

private:
	Scene *pTargetScene;
	
	void INIT(GLuint width, GLuint height);

};

