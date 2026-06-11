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
	glm::mat4 model_matrix;
	cs300::Mesh* mesh;
	bool is_light;

	struct {
		glm::vec3 pos;
		glm::vec3 rot;
		glm::vec3 sca;
	} transform;

	CS300Parser::Transform object;
	CS300Parser::Light light;

	Object(const CS300Parser::Transform& transform_data) {
		is_light = false;
		this->mesh = Manager::getMesh(transform_data.mesh);
		if (!this->mesh) {
			std::cerr << "Null resource in object: " << this->name << '\n';
		}
		name = transform_data.name;
		transform.pos = transform_data.pos;
		transform.rot = transform_data.rot;
		transform.sca = transform_data.sca;
		object = transform_data;
		rebuildMatrix();
	}

	Object(const CS300Parser::Light& light_data) : light(light_data) {
		is_light = true;
		name = "Light";
		transform.pos = light_data.pos;
		transform.rot = glm::vec3(0.0f);
		transform.sca = glm::vec3(1.0f);
		mesh = nullptr;
		light = light_data;
		rebuildMatrix();
	}

	void update(float dt) {
		float time = dt;
		if (not is_light) {
			glm::vec3 current = transform.pos;
			for (const auto& anim : object.anims) {
				current = anim.Update(object.pos, time);
			}
			transform.pos = current;
		} else {
			glm::vec3 current = light.pos;
			for (const auto& anim : light.anims) {
				current = anim.Update(light.pos, time);
			}
			transform.pos = current;
		}
		rebuildMatrix();
	}

	void rebuildMatrix() {
		model_matrix = glm::identity<glm::mat4>();
		model_matrix = glm::translate(model_matrix, transform.pos);
		glm::quat quaternion_rotation = glm::quat(glm::radians(transform.rot));
		model_matrix = model_matrix * glm::mat4_cast(quaternion_rotation);
		model_matrix = glm::scale(model_matrix, transform.sca);
	}
};
