#include "Object.hpp"

#include "Shader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <utility>

Object::Object(
    std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> normal, const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& sca,
    float shininess, const std::vector<Animations::Anim>& anims
) {
	m.mesh = std::move(mesh);
	m.normal = std::move(normal);
	m.original_pos = pos;
	m.curr_pos = pos;
	m.rot = rot;
	m.sca = sca;
	m.shininess = shininess;
	m.anims = anims;
}

void Object::update(float time) {
	m.curr_pos = Animations::Apply(m.original_pos, m.anims, time);
}

void Object::draw(const Shader& shader, const glm::mat4& view_proj, bool averaged_normals, bool draw_textures) const {
	if (!m.mesh) {
		return;
	}
	if (m.normal) {
		m.normal->bind(1);
		shader.setUniform("uNormalTex", 1);
		shader.setUniform("uUseNormalTex", true);
	} else {
		shader.setUniform("uUseNormalTex", false);
	}
	glm::mat4 model = getModelMatrix();
	shader.setUniform("uModel", model);
	shader.setUniform("uMVP", view_proj * model);
	shader.setUniform("uNormalMatrix", glm::transpose(glm::inverse(model)));

	shader.setUniform("uUseDiffuseTex", draw_textures);
	shader.setUniform("uShininess", m.shininess);

	m.mesh->draw(averaged_normals);
}

void Object::drawLines(const Shader& shader, const glm::mat4& view_proj, bool averaged_normals) const {
	if (!m.mesh) {
		return;
	}

	glm::mat4 model = getModelMatrix();
	shader.setUniform("uMVP", view_proj * model);
	glUniform4f(1, 0.0f, 0.0f, 1.0f, 1.0f); 
	m.mesh->drawNormals(averaged_normals);
	glUniform4f(1, 1.0f, 0.0f, 0.0f, 1.0f); 
	m.mesh->drawTangents(averaged_normals);
	glUniform4f(1, 0.0f, 1.0f, 0.0f, 1.0f); 
	m.mesh->drawBitangents(averaged_normals);
}

auto Object::getModelMatrix() const -> glm::mat4 {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m.curr_pos);
	model = glm::rotate(model, glm::radians(m.rot.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(m.rot.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(m.rot.z), glm::vec3(0, 0, 1));
	model = glm::scale(model, m.sca);
	return model;
}
