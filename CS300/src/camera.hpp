#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"

#include <algorithm>

class Camera {
	struct {
		glm::vec3 target;
		glm::vec3 world_up;

		float r;
		float alpha;
		float beta;

		float fov;
		float width;
		float height;
		float near_plane;
		float far_plane;
	} m;

public:
	[[nodiscard]]
	auto getPosition() const -> glm::vec3 {
		glm::vec3 pos;
		// Standard spherical-to-cartesian coordinate conversion based on your diagram
		pos.x = m.r * sin(glm::radians(m.alpha)) * sin(glm::radians(m.beta));
		pos.y = m.r * cos(glm::radians(m.alpha));
		pos.z = m.r * sin(glm::radians(m.alpha)) * cos(glm::radians(m.beta));

		// Offset by target if the target isn't precisely at (0,0,0)
		return pos + m.target;
	}

	[[nodiscard]]
	auto getViewMatrix() const -> glm::mat4 {
		// Looks from the calculated spherical position, directly AT the target
		return glm::lookAt(getPosition(), m.target, m.world_up);
	}

	[[nodiscard]]
	auto getProjectionMatrix() const -> glm::mat4 {
		float aspect_ratio = m.width / m.height;
		return glm::perspective(glm::radians(m.fov), aspect_ratio, m.near_plane, m.far_plane);
	}

	void modifyAlpha(float delta) { m.alpha += delta; }    // W/S

	void modifyBeta(float delta) { m.beta += delta; }      // A/D

	void modifyRadius(float delta) {
		m.r += delta;
		m.r = std::max(m.r, 0.1f);
	}
};
