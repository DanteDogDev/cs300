#include "cs300/CS300Parser.h"
#include "mesh.hpp"
#include "resources.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

class Object {
public:
	std::string name;

	glm::mat4 model_matrix;

	cs300::Mesh* mesh;
	cs300::Shader* shader;
	cs300::Texture* texture;

	Object(const CS300Parser::Transform& transform_data, std::string mesh) : name(transform_data.name) {
		this->mesh = ResourceManager::instance().getMesh(mesh);
		this->shader = ResourceManager::instance().getShader("default");
		this->texture = ResourceManager::instance().getTexture("default");
		if (!this->mesh || !this->shader || !this->texture) {
			std::cerr << "Null resource in object: " << this->name << '\n';
		}
		model_matrix = glm::identity<glm::mat4>();
		model_matrix = glm::translate(model_matrix, transform_data.pos);
		model_matrix = glm::scale(model_matrix, transform_data.sca);
	}
};
