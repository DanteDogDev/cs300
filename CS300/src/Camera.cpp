#include "Camera.hpp"

Camera::Camera(float fov, float aspect, float near, float far) {
	m.fov = fov;
	m.aspect = aspect;
	m.near = near;
	m.far = far;
	m.proj = glm::perspective(glm::radians(m.fov), m.aspect, m.near, m.far);
}

void Camera::update(float r, float alpha, float beta, const glm::vec3& target) {
	float x = r * sin(glm::radians(alpha)) * sin(glm::radians(beta));
	float y = r * cos(glm::radians(alpha));
	float z = r * sin(glm::radians(alpha)) * cos(glm::radians(beta));

	m.pos = target + glm::vec3(x, y, z);
	m.view = glm::lookAt(m.pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

auto Camera::getView() const -> glm::mat4 {
	return m.view;
}

auto Camera::getProj() const -> glm::mat4 {
	return m.proj;
}

auto Camera::getPosition() const -> glm::vec3 {
	return m.pos;
}
