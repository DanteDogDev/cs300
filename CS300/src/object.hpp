#include "cs300/CS300Parser.h"
#include "cs300/animations.h"
#include "generate.hpp"
#include "glm/gtc/quaternion.hpp"
#include "mesh.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <pthread.h>
#include <string>

class Object {
public:
	std::string name;
	glm::vec3 origin_position;
	std::vector<Animations::Anim> anims;
	glm::mat4 model_matrix;
	cs300::Mesh* mesh;

	struct {
		glm::vec3 pos;
		glm::vec3 rot;
		glm::vec3 sca;
	} transform;

	struct {
	} material;



	Object(const CS300Parser::Transform& transform_data, const std::string& mesh) : name(transform_data.name) {
		this->mesh = Manager::getMesh(mesh);
		if (!this->mesh) {
			std::cerr << "Null resource in object: " << this->name << '\n';
		}
		transform.pos = transform_data.pos;
		transform.rot = transform_data.rot;
		transform.sca = transform_data.sca;
		anims = transform_data.anims;
		origin_position = transform.pos;
		rebuildMatrix();
	}

	void update(float dt) {
		float time = dt;
		auto current = transform.pos;
		for (const auto & anim : anims) {
				current = anim.Update(origin_position, time);
		}
		if (not anims.empty()) {
			transform.pos = current;
			rebuildMatrix();
		}
	}

	void rebuildMatrix() {
		model_matrix = glm::identity<glm::mat4>();
		model_matrix = glm::translate(model_matrix, transform.pos);
		glm::quat quaternion_rotation = glm::quat(glm::radians(transform.rot));
		model_matrix = model_matrix * glm::mat4_cast(quaternion_rotation);
		model_matrix = glm::scale(model_matrix, transform.sca);
	}
};
