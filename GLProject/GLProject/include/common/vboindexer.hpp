#ifndef VBOINDEXER_HPP
#define VBOINDEXER_HPP

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
);

void indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec3> & in_normals,

	std::vector<unsigned int> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec3> & out_normals
);

#endif