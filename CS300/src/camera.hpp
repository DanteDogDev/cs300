#include "cs300/CS300Parser.h"    // Include CS300Parser.h
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"

#include <algorithm>

class Camera {
	struct {
		glm::vec3 position;    // New: direct camera position
		glm::vec3 target;
		glm::vec3 world_up;

		float fov;
		float width;
		float height;
		float near_plane;
		float far_plane;
	} m;

public:
	Camera(
	    float fov, float width, float height, float near_plane, float far_plane, glm::vec3 camPos, glm::vec3 camTarget,
	    glm::vec3 camUp
	) {
		m.fov = fov;
		m.width = width;
		m.height = height;
		m.near_plane = near_plane;
		m.far_plane = far_plane;

		m.position = camPos;
		m.target = camTarget;
		m.world_up = camUp;
	}

	// New constructor taking CS300Parser object
	Camera(const CS300Parser& parser)
	    : Camera(
	          parser.fovy, parser.width, parser.height, parser.nearPlane, parser.farPlane, parser.camPos, parser.camTarget,
	          parser.camUp
	      ) { }

	[[nodiscard]]
	auto getPosition() const -> glm::vec3 {
		return m.position;
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

	void modifyAlpha(float delta) {
		glm::vec3 relativePos = m.position - m.target;
		float r = glm::length(relativePos);
		float alpha = glm::degrees(acos(relativePos.y / r));
		float beta = glm::degrees(atan2(relativePos.z, relativePos.x));

		alpha += delta;
		alpha = std::clamp(alpha, 0.1f, 179.9f);    // Keep alpha within reasonable bounds

		// Convert back to Cartesian
		relativePos.x = r * sin(glm::radians(alpha)) * cos(glm::radians(beta));
		relativePos.y = r * cos(glm::radians(alpha));
		relativePos.z = r * sin(glm::radians(alpha)) * sin(glm::radians(beta));

		m.position = m.target + relativePos;
	}

	void modifyBeta(float delta) {
		glm::vec3 relativePos = m.position - m.target;
		float r = glm::length(relativePos);
		float alpha = glm::degrees(acos(relativePos.y / r));
		float beta = glm::degrees(atan2(relativePos.z, relativePos.x));

		beta += delta;

		// Convert back to Cartesian
		relativePos.x = r * sin(glm::radians(alpha)) * cos(glm::radians(beta));
		relativePos.y = r * cos(glm::radians(alpha));
		relativePos.z = r * sin(glm::radians(alpha)) * sin(glm::radians(beta));

		m.position = m.target + relativePos;
	}

	void modifyRadius(float delta) {
		glm::vec3 relativePos = m.position - m.target;
		float r = glm::length(relativePos);
		float alpha = glm::degrees(acos(relativePos.y / r));
		float beta = glm::degrees(atan2(relativePos.z, relativePos.x));

		r += delta;
		r = std::max(r, 0.1f);    // Ensure radius is not too small

		// Convert back to Cartesian
		relativePos.x = r * sin(glm::radians(alpha)) * cos(glm::radians(beta));
		relativePos.y = r * cos(glm::radians(alpha));
		relativePos.z = r * sin(glm::radians(alpha)) * sin(glm::radians(beta));

		m.position = m.target + relativePos;
	}
};
