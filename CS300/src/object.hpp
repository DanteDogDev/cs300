#include "mesh.hpp"
#include "shader.hpp"
#include "resources.hpp" // Include ResourceManager

#include <string>

class Object {
public:
	// Constructor taking resource pointers
	Object(const std::string& name, Mesh* mesh, Shader* shader)
	    : name(name), mesh(mesh), shader(shader) {}

	// New simplified constructor taking string paths for resources
	Object(const std::string& name, const std::string& meshPath,
	       const std::string& shaderName)
	    : Object(name, ResourceManager::instance().getMesh(meshPath),
	             ResourceManager::instance().getShader(shaderName)) {}

	void draw() const {
		if (shader && mesh) {
			shader->bind();
			mesh->draw();
			shader->unbind();
		}
	}

	std::string name;

	struct {
	} transform;

	Mesh* mesh;
	Shader* shader;
};
