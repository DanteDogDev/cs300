#include "cs300/CS300Parser.h"    
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

class Camera {
	struct {
		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 world_up;

		float fov;
		float width;
		float height;
		float near_plane;
		float far_plane;

	} m;

public:
	float radius;
	float alpha;
	float beta;

	Camera(const CS300Parser& parser) {
		m.fov = parser.fovy;
		m.width = parser.width;
		m.height = parser.height;
		m.near_plane = parser.nearPlane;
		m.far_plane = parser.farPlane;

		m.position = parser.camPos;
		m.target = parser.camTarget;
		m.world_up = parser.camUp;

		glm::vec3 delta = m.position - m.target;
		radius = glm::length(delta);
		alpha = glm::asin(-delta.y / radius);
		beta = atan2(delta.x, delta.z) + glm::radians(180.0f);
	}

	void updatePosition() {
		alpha = glm::clamp(alpha, glm::radians(-89.0f), glm::radians(89.0f));
		float x = radius * glm::cos(alpha) * glm::sin(beta);
		float y = -radius * glm::sin(alpha);
		float z = radius * glm::cos(alpha) * glm::cos(beta);

		m.position = m.target + glm::vec3(x, y, z);
	}

	[[nodiscard]]
	auto getViewMatrix() const -> glm::mat4 {
		return glm::lookAt(m.position, m.target, m.world_up);
	}

	[[nodiscard]]
	auto getProjectionMatrix() const -> glm::mat4 {
		float aspect_ratio = m.width / m.height;
		return glm::perspective(glm::radians(m.fov), aspect_ratio, m.near_plane, m.far_plane);
	}
};
