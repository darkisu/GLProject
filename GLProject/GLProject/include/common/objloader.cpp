#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>

#include <vector>
#include <glm/glm.hpp>

// Include GLEW
#include <GLEW/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "texture.hpp"
#include "objloader.hpp"

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool loadOBJ(
	const aiScene* scene,
	const aiNode* nd,
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec3> & out_normals
){
	aiMatrix4x4 m = nd->mTransformation;
	aiTransposeMatrix4(&m);
	
	ilInit();

	for (int n=0; n < nd ->mNumMeshes; ++n) {

		std::vector  <glm::vec3> temp_point;
		//std::cout<<nd->mMeshes[0]<<std::endl;

		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		const aiMaterial* pMaterial=scene->mMaterials[mesh->mMaterialIndex];

		aiColor4D diffuse;
		aiColor4D specular;
	    aiColor4D ambient;

		int count=0;

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (int t = 0; t < mesh->mNumVertices; ++t) {

			glm::vec3 vertex;

			vertex.x=mesh->mVertices[t].x;
			vertex.y=mesh->mVertices[t].y;
			vertex.z=mesh->mVertices[t].z;

			temp_point.push_back(vertex);

			//temp_normal.push_back(normal);
			//out_normals.push_back(normal);

			//std::cout<<mesh->mVertices[t].x<<" "<<mesh->mVertices[t].y<<" "<<mesh->mVertices[t].z<<" "<<count<<std::endl;
			//std::cout<<mesh->mNormals[t].x<<" "<<mesh->mNormals[t].y<<" "<<mesh->mNormals[t].z<<" "<<count<<std::endl;
			//std::cout<<pTexCoord->x<<" "<<pTexCoord->y<<" "<<pTexCoord->z<<" "<<count<<std::endl;

			count++;
		}

		for (unsigned int i = 0 ; i <  mesh->mNumFaces ; i++) { 
			const aiFace& Face = mesh->mFaces[i]; 
			assert(Face.mNumIndices == 3);

			if(Face.mNumIndices!=3){
				std::cout<<"!!!!!!!!!!!!!!!!!!!"<<std::endl;
			}
			out_vertices.push_back(temp_point[Face.mIndices[0]]);
			out_vertices.push_back(temp_point[Face.mIndices[1]]);
			out_vertices.push_back(temp_point[Face.mIndices[2]]);

			//std::cout<<Face.mIndices[0]<<" "<<Face.mIndices[1]<<" "<<Face.mIndices[2]<<" "<<std::endl;
			//std::cout<<Face.<<" "<<Face.mIndices[1]<<" "<<Face.mIndices[2]<<" "<<std::endl;

			for(int j = 0; j < Face.mNumIndices; j++) {
				glm::vec3 normal;
				glm::vec2 uv;
				glm::vec3 Ka;
				glm::vec3 Kd;
				glm::vec3 Ks;

				int index = Face.mIndices[j];
				
				//std::cout<<index<<std::endl;
				normal.x=mesh->mNormals[index].x;
				normal.y=mesh->mNormals[index].y;
				normal.z=mesh->mNormals[index].z;
				out_normals.push_back(normal);
			}
		}		
		temp_point.clear();
		//std::cout<<"---------------------------------------"<<n<<std::endl;
	}

	for (int n = 0; n < nd->mNumChildren; ++n) {
		loadOBJ(scene,
			    nd->mChildren[n], 
				out_vertices, 
				out_normals);
	}

	return true;
}

bool loadOBJ(
	const aiScene* scene,
	const aiNode* nd,
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	std::vector<glm::vec3> & out_Ka, 
	std::vector<glm::vec3> & out_Kd,
	std::vector<glm::vec3> & out_Ks
){
	aiMatrix4x4 m = nd->mTransformation;
	aiTransposeMatrix4(&m);
	
	ilInit();

	for (int n=0; n < nd ->mNumMeshes; ++n) {

		std::vector  <glm::vec3> temp_point;
		//std::cout<<nd->mMeshes[0]<<std::endl;

		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		const aiMaterial* pMaterial=scene->mMaterials[mesh->mMaterialIndex];

		aiColor4D diffuse;
		aiColor4D specular;
	    aiColor4D ambient;

		int count=0;

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (int t = 0; t < mesh->mNumVertices; ++t) {

			glm::vec3 vertex;

			vertex.x=mesh->mVertices[t].x;
			vertex.y=mesh->mVertices[t].y;
			vertex.z=mesh->mVertices[t].z;

			temp_point.push_back(vertex);

			//temp_normal.push_back(normal);
			//out_normals.push_back(normal);

			//std::cout<<mesh->mVertices[t].x<<" "<<mesh->mVertices[t].y<<" "<<mesh->mVertices[t].z<<" "<<count<<std::endl;
			//std::cout<<mesh->mNormals[t].x<<" "<<mesh->mNormals[t].y<<" "<<mesh->mNormals[t].z<<" "<<count<<std::endl;
			//std::cout<<pTexCoord->x<<" "<<pTexCoord->y<<" "<<pTexCoord->z<<" "<<count<<std::endl;

			count++;
		}

		for (unsigned int i = 0 ; i <  mesh->mNumFaces ; i++) { 
			const aiFace& Face = mesh->mFaces[i]; 
			assert(Face.mNumIndices == 3);

			if(Face.mNumIndices!=3){
				std::cout<<"!!!!!!!!!!!!!!!!!!!"<<std::endl;
			}
			out_vertices.push_back(temp_point[Face.mIndices[0]]);
			out_vertices.push_back(temp_point[Face.mIndices[1]]);
			out_vertices.push_back(temp_point[Face.mIndices[2]]);

			//std::cout<<Face.mIndices[0]<<" "<<Face.mIndices[1]<<" "<<Face.mIndices[2]<<" "<<std::endl;
			//std::cout<<Face.<<" "<<Face.mIndices[1]<<" "<<Face.mIndices[2]<<" "<<std::endl;

			for(int j = 0; j < Face.mNumIndices; j++) {
				glm::vec3 normal;
				glm::vec2 uv;
				glm::vec3 Ka;
				glm::vec3 Kd;
				glm::vec3 Ks;

				int index = Face.mIndices[j];
				
				//std::cout<<index<<std::endl;
				normal.x=mesh->mNormals[index].x;
				normal.y=mesh->mNormals[index].y;
				normal.z=mesh->mNormals[index].z;

				uv.x=mesh->mTextureCoords[0][index].x;
				uv.y=-mesh->mTextureCoords[0][index].y;

				//std::cout<<mesh->mNormals[index].x<<" "<<mesh->mNormals[index].y<<" "<<mesh->mNormals[index].z<<std::endl;
				//std::cout<<index<<std::endl;
				//std::cout<<mesh->mTextureCoords[0][index].x<<" "<<mesh->mTextureCoords[0][index].y<<std::endl;

				if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient)){
					//std::cout<<ambient.r<<" "<<ambient.g<<" "<<ambient.b<<" "<<std::endl;
					Ka.x=ambient.r;
					Ka.y=ambient.g;
					Ka.z=ambient.b;
				}

				if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse)){
					//std::cout<<diffuse.r<<" "<<diffuse.g<<" "<<diffuse.b<<std::endl;
					Kd.x=diffuse.r;
					Kd.y=diffuse.g;
					Kd.z=diffuse.b;
				}

				if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &specular)){
					//std::cout<<specular.r<<" "<<specular.g<<" "<<specular.b<<std::endl;
					Ks.x=specular.r;
					Ks.y=specular.g;
					Ks.z=specular.b;
				}

				out_normals.push_back(normal);
				out_uvs.push_back(uv);

				out_Ka.push_back(Ka);
				out_Kd.push_back(Kd);
				out_Ks.push_back(Ks);
			}
			
		}		

		temp_point.clear();

		//std::cout<<"---------------------------------------"<<n<<std::endl;
	}

	for (int n = 0; n < nd->mNumChildren; ++n) {
		loadOBJ(scene,
			    nd->mChildren[n], 
				out_vertices, 
				out_uvs,
				out_normals,
				out_Ka, 
				out_Kd,
				out_Ks);
	}

	return true;
}

bool loadOBJ(
	const aiScene* scene,
	const aiNode* nd,
	std::vector<ModelMesh> & out_modelmesh,
	std::vector<ModelMaterial> & out_modelmaterial
){
	aiMatrix4x4 m = nd->mTransformation;
	aiTransposeMatrix4(&m);
	
	ilInit();

	for (int n=0; n < nd ->mNumMeshes; ++n) {

		ModelMesh modelMesh;

		std::vector  <glm::vec3> temp_point;
		//std::cout<<nd->mMeshes[0]<<std::endl;

		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		const aiMaterial* pMaterial=scene->mMaterials[mesh->mMaterialIndex];

		//-----------------------------------------------------------------------------------------------------
		
		aiString name;
		bool isSaveMaterial = false;
		pMaterial->Get(AI_MATKEY_NAME,name);
		for(int i = 0 ; i < out_modelmaterial.size() ; i++)
		{
			if(strcmp (name.C_Str(),out_modelmaterial[i].Name) == 0)
			{
				modelMesh.indexModelMaterial = i;
				isSaveMaterial = true;
				break;
			}
		}

		if(!isSaveMaterial)
		{
			ModelMaterial modelMaterial;
			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString Path;
				if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					std::string FullPath =  Path.data;
					FullPath="model\\"+FullPath;
					ILboolean result = ilLoadImage(FullPath.c_str());
					if( result == true ){
						//printf("the image loaded successfully\n");
						modelMaterial.MapKd = loadImage(FullPath.c_str());
						strcpy(modelMaterial.Name,name.C_Str());
					}
					else
					{
						printf("The image failed to load\n" ) ;
						ILenum err = ilGetError() ;
						printf( "the error %d\n", err );
						printf( "string is %s\n", ilGetString( err ) );
					}
				}
			}
			out_modelmaterial.push_back(modelMaterial);
			modelMesh.indexModelMaterial = out_modelmaterial.size()-1;
		}
		
		aiColor4D diffuse;
		aiColor4D specular;
	    aiColor4D ambient;

		int count=0;

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (int t = 0; t < mesh->mNumVertices; ++t) {

			glm::vec3 vertex;

			vertex.x=mesh->mVertices[t].x;
			vertex.y=mesh->mVertices[t].y;
			vertex.z=mesh->mVertices[t].z;

			temp_point.push_back(vertex);

			//temp_normal.push_back(normal);
			//out_normals.push_back(normal);

			//std::cout<<mesh->mVertices[t].x<<" "<<mesh->mVertices[t].y<<" "<<mesh->mVertices[t].z<<" "<<count<<std::endl;
			//std::cout<<mesh->mNormals[t].x<<" "<<mesh->mNormals[t].y<<" "<<mesh->mNormals[t].z<<" "<<count<<std::endl;
			//std::cout<<pTexCoord->x<<" "<<pTexCoord->y<<" "<<pTexCoord->z<<" "<<count<<std::endl;

			count++;
		}

		for (unsigned int i = 0 ; i <  mesh->mNumFaces ; i++) { 
			const aiFace& Face = mesh->mFaces[i]; 
			assert(Face.mNumIndices == 3);

			if(Face.mNumIndices!=3){
				std::cout<<"!!!!!!!!!!!!!!!!!!!"<<std::endl;
			}
			modelMesh.Vertices.push_back(temp_point[Face.mIndices[0]]);
			modelMesh.Vertices.push_back(temp_point[Face.mIndices[1]]);
			modelMesh.Vertices.push_back(temp_point[Face.mIndices[2]]);

			//std::cout<<Face.mIndices[0]<<" "<<Face.mIndices[1]<<" "<<Face.mIndices[2]<<" "<<std::endl;
			//std::cout<<Face.<<" "<<Face.mIndices[1]<<" "<<Face.mIndices[2]<<" "<<std::endl;

			for(int j = 0; j < Face.mNumIndices; j++) {
				glm::vec3 normal;
				glm::vec2 uv;
				glm::vec3 Ka;
				glm::vec3 Kd;
				glm::vec3 Ks;

				int index = Face.mIndices[j];
				
				//std::cout<<index<<std::endl;
				normal.x=mesh->mNormals[index].x;
				normal.y=mesh->mNormals[index].y;
				normal.z=mesh->mNormals[index].z;

				uv.x=mesh->mTextureCoords[0][index].x;
				uv.y=mesh->mTextureCoords[0][index].y;

				//std::cout<<mesh->mNormals[index].x<<" "<<mesh->mNormals[index].y<<" "<<mesh->mNormals[index].z<<std::endl;
				//std::cout<<index<<std::endl;
				//std::cout<<mesh->mTextureCoords[0][index].x<<" "<<mesh->mTextureCoords[0][index].y<<std::endl;

				if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient)){
					//std::cout<<ambient.r<<" "<<ambient.g<<" "<<ambient.b<<" "<<std::endl;
					Ka.x=ambient.r;
					Ka.y=ambient.g;
					Ka.z=ambient.b;
				}

				if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse)){
					//std::cout<<diffuse.r<<" "<<diffuse.g<<" "<<diffuse.b<<std::endl;
					Kd.x=diffuse.r;
					Kd.y=diffuse.g;
					Kd.z=diffuse.b;
				}

				if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &specular)){
					//std::cout<<specular.r<<" "<<specular.g<<" "<<specular.b<<std::endl;
					Ks.x=specular.r;
					Ks.y=specular.g;
					Ks.z=specular.b;
				}

				modelMesh.Normals.push_back(normal);
				modelMesh.UVs.push_back(uv);

				modelMesh.Ka.push_back(Ka);
				modelMesh.Kd.push_back(Kd);
				modelMesh.Ks.push_back(Ks);
				
			}
			
		}		
		
		out_modelmesh.push_back(modelMesh);
		temp_point.clear();

		//std::cout<<"---------------------------------------"<<n<<std::endl;
	}

	for (int n = 0; n < nd->mNumChildren; ++n) {
		loadOBJ(scene,
			    nd->mChildren[n], 
				out_modelmesh,
				out_modelmaterial);
	}
	return true;
}