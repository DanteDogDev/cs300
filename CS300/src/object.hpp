#include "cs300/CS300Parser.h"             // For CS300Parser::Transform
#include "mesh.hpp"
#include "resources.hpp"                   // Include ResourceManager
#include "shader.hpp"

#include <glm/gtc/matrix_transform.hpp>    // For glm::translate, glm::scale
#include <string>
#include <utility>

class Object {
public:
	std::string name;

	glm::mat4 model_matrix;    // New: Model matrix for transformations

	Mesh* mesh;
	Shader* shader;

	Object(const CS300Parser::Transform& transform_data, std::string mesh) : name(transform_data.name) {
		this->mesh = ResourceManager::instance().getMesh(mesh);
		this->shader = ResourceManager::instance().getShader("default");
		model_matrix = glm::identity<glm::mat4>();
		model_matrix = glm::translate(model_matrix, transform_data.pos);
		model_matrix = glm::scale(model_matrix, transform_data.sca);
	}

	void draw() const {
		if (shader && mesh) {
			shader->bind();
			mesh->draw();
			shader->unbind();
		}
	}
};
