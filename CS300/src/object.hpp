#include "mesh.hpp"
#include "shader.hpp"

#include <string>

class Object {
	std::string name;

	struct {
	} transform;

	Mesh* mesh;
	Shader* shader;
};
