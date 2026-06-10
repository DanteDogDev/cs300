#include "cs300/CS300Parser.h"
#include "generate.hpp"
#include "mesh.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

class Object {
public:
	std::string name;

	glm::mat4 model_matrix;

	cs300::Mesh* mesh;
	gl::Texture* texture;

	Object(const CS300Parser::Transform& transform_data, const std::string& mesh) : name(transform_data.name) {
		this->mesh = Manager::getMesh(mesh);
		this->texture = Manager::getTexture("./data/textures/default.bmp");
		if (!this->mesh || !this->texture) {
			std::cerr << "Null resource in object: " << this->name << '\n';
		}
		model_matrix = glm::identity<glm::mat4>();
		model_matrix = glm::translate(model_matrix, transform_data.pos);
		model_matrix = glm::scale(model_matrix, transform_data.sca);
	}
};
