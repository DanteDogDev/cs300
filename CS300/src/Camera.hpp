#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	friend auto main(int argc, char* args[]) -> int;

public:
	Camera() = default;
	Camera(float fov, float aspect, float near, float far);
	Camera(glm::vec3 pos, float fov, float aspect, float near, float far);
	Camera(glm::vec3 pos, glm::vec3 view, float fov, float aspect, float near, float far);

	void update(float r, float alpha, float beta, const glm::vec3& target);

	auto getView() const -> glm::mat4;
	auto getProj() const -> glm::mat4;
	auto getPosition() const -> glm::vec3;

private:
	struct {
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		glm::vec3 pos = glm::vec3(0.0f);
		float fov;
		float aspect;
		float near;
		float far;
	} m;
};
