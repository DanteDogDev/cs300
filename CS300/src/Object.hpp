#pragma once

#include "Mesh.hpp"
#include "cs300/animations.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Shader;

class Object {
public:
	Object(
	    std::shared_ptr<Mesh> mesh, const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& rot = glm::vec3(0.0f),
	    const glm::vec3& sca = glm::vec3(1.0f), float shininess = 10.0f, const std::vector<Animations::Anim>& anims = {}
	);

	void draw(const Shader& shader, const glm::mat4& view_proj, bool averaged_normals, bool draw_textures) const;
	void drawNormals(const Shader& shader, const glm::mat4& view_proj, bool averaged_normals) const;

	void update(float time);

	void setRotation(const glm::vec3& rot) { m.rot = rot; }

	void setScale(const glm::vec3& sca) { m.sca = sca; }

	auto getModelMatrix() const -> glm::mat4;

private:
	struct {
		std::shared_ptr<Mesh> mesh;
		glm::vec3 original_pos;
		glm::vec3 curr_pos;
		glm::vec3 rot;
		glm::vec3 sca;
		float shininess = 10.0f;
		std::vector<Animations::Anim> anims;
	} m;
};
