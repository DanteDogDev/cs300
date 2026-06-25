#include "Light.hpp"

#include "Shader.hpp"

#include <glm/gtc/constants.hpp>

namespace {
auto field(int index, const char* name) -> std::string {
	return "uLight[" + std::to_string(index) + "]." + name;
}
}

Light::Light(const CS300Parser::Light& data) {
	m.color = data.col;
	m.ambient = data.amb;
	m.original_pos = data.pos;
	m.curr_pos = data.pos;
	m.anims = data.anims;
}

auto Light::create(const CS300Parser::Light& data) -> std::unique_ptr<Light> {
	if (data.type == "DIR") {
		return std::make_unique<DirectionalLight>(data);
	}
	if (data.type == "SPOT") {
		return std::make_unique<SpotLight>(data);
	}
	return std::make_unique<PointLight>(data);
}

void Light::update(float time) {
	m.curr_pos = Animations::Apply(m.original_pos, m.anims, time);
}

void Light::setUniforms(const Shader& shader, int index) const {
	shader.setUniform(field(index, "type"), type());
	shader.setUniform(field(index, "position"), m.curr_pos);

	shader.setUniform(field(index, "ambient"), m.color * m.ambient);
	shader.setUniform(field(index, "diffuse"), m.color);
	shader.setUniform(field(index, "specular"), glm::vec3(1.0f));

	shader.setUniform(field(index, "bias"), m.bias);
	shader.setUniform(field(index, "pcf"), m.pcf);
}

PointLight::PointLight(const CS300Parser::Light& data) : Light(data), m_attenuation(data.att) { }

void PointLight::setUniforms(const Shader& shader, int index) const {
	Light::setUniforms(shader, index);
	shader.setUniform(field(index, "attenuation"), m_attenuation);
}

DirectionalLight::DirectionalLight(const CS300Parser::Light& data) : Light(data), m_direction(data.dir) { }

void DirectionalLight::setUniforms(const Shader& shader, int index) const {
	Light::setUniforms(shader, index);
	shader.setUniform(field(index, "direction"), m_direction);
}

SpotLight::SpotLight(const CS300Parser::Light& data)
    : Light(data),
      m_direction(data.dir),
      m_attenuation(data.att),
      m_inner(glm::radians(data.inner)),
      m_outer(glm::radians(data.outer)),
      m_falloff(data.falloff) { }

void SpotLight::setUniforms(const Shader& shader, int index) const {
	Light::setUniforms(shader, index);
	shader.setUniform(field(index, "direction"), m_direction);
	shader.setUniform(field(index, "attenuation"), m_attenuation);
	shader.setUniform(field(index, "innerAngle"), m_inner);
	shader.setUniform(field(index, "outerAngle"), m_outer);
	shader.setUniform(field(index, "falloff"), m_falloff);
}
