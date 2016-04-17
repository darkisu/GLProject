#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ModelMaterial.h"
#include "ModelMesh.h"

bool loadOBJ(
	const aiScene* scene,
	const aiNode* nd,
	std::vector<glm::vec3> & out_vertices,  
	std::vector<glm::vec3> & out_normals
);

bool loadOBJ(
	const aiScene* scene,
	const aiNode* nd,
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals,
	std::vector<glm::vec3> & out_Ka, 
	std::vector<glm::vec3> & out_Kd,
	std::vector<glm::vec3> & out_Ks
);

bool loadOBJ(
	const aiScene* scene,
	const aiNode* nd,
	std::vector<ModelMesh> & out_modelmesh,
	std::vector<ModelMaterial> & out_modelmaterial
);

#endif