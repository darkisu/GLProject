#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "vboindexer.hpp"

#include <string.h> // for memcmp

struct PackedVertex{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 ka;
	glm::vec3 kd;
	glm::vec3 ks;
	bool operator<(const PackedVertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};

bool getSimilarVertexIndex_fast( 
	PackedVertex & packed, 
	std::map<PackedVertex,unsigned int> & VertexToOutIndex,
	unsigned int & result
){
	std::map<PackedVertex,unsigned int>::iterator it = VertexToOutIndex.find(packed);
	if ( it == VertexToOutIndex.end() ){
		return false;
	}else{
		result = it->second;
		return true;
	}
}

void indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,

	std::vector<glm::vec3> & in_ka,
	std::vector<glm::vec3> & in_kd,
	std::vector<glm::vec3> & in_ks,

	std::vector<unsigned int> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,

	std::vector<glm::vec3> & out_ka,
	std::vector<glm::vec3> & out_kd,
	std::vector<glm::vec3> & out_ks
){
	std::map<PackedVertex,unsigned int> VertexToOutIndex;

	// For each input vertex
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){

		PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i], in_ka[i], in_kd[i], in_ks[i]};
		
		// Try to find a similar vertex in out_XXXX
		unsigned int index;
		
		bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );
		}else{ // If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			out_uvs     .push_back( in_uvs[i]);
			out_normals .push_back( in_normals[i]);

			out_ka.push_back( in_ka[i]);
			out_kd.push_back( in_kd[i]);
			out_ks.push_back( in_ks[i]);

			unsigned int newindex = (unsigned int)out_vertices.size() - 1;
			out_indices .push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}

void indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec3> & in_normals,

	std::vector<unsigned int> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec3> & out_normals
){
	std::map<PackedVertex,unsigned int> VertexToOutIndex;

	// For each input vertex
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){

		glm::vec2 v2temp = glm::vec2(0.0f,0.0f);
		glm::vec3 v3temp = glm::vec3(0.0f,0.0f,0.0f);

		PackedVertex packed = {in_vertices[i], v2temp, in_normals[i], v3temp, v3temp, v3temp};
		
		// Try to find a similar vertex in out_XXXX
		unsigned int index;
		
		bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );
		}else{ // If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			out_normals .push_back( in_normals[i]);
			unsigned int newindex = (unsigned int)out_vertices.size() - 1;
			out_indices .push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}