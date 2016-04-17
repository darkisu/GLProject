#pragma once

#include "Mesh.h"
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


GLint TextureFromFile(const char* path, string directory);

typedef struct PointOfLight
{
	glm::vec3 position;
	glm::vec3 color;
};


class Scene
{
public:
	Scene(GLchar* path)
	{
		pointOfLight.position = glm::vec3(0.0, 0.0, 0.0);
		pointOfLight.color = glm::vec3(1.0, 1.0, 1.0);
		this->loadModel(path);
	}
	~Scene();
	void Draw(Shader shader);
	void loadModel(string path);
	void setupPointOfLight(glm::vec3 position, glm::vec3 color)
	{
		pointOfLight.position = position;
		pointOfLight.color = color;
	}
private:
	/*  Model Data  */
	PointOfLight pointOfLight;
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

										// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	


	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

};
