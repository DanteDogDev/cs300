#pragma once

#include "cs300/CS300Parser.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Shader;

constexpr int LIGHT_NUM_MAX = 8;

class Light {
public:
	explicit Light(const CS300Parser::Light& data);
	virtual ~Light() = default;

	static auto create(const CS300Parser::Light& data) -> std::unique_ptr<Light>;
	virtual auto type() const -> int = 0;
	virtual void setUniforms(const Shader& shader, int index) const;
	void update(float time);

protected:
	struct {
		glm::vec3 color;
		float ambient = .2f;

		glm::vec3 original_pos;
		glm::vec3 curr_pos;

		std::vector<Animations::Anim> anims;
	} m;
};

class PointLight : public Light {
public:
	explicit PointLight(const CS300Parser::Light& data);

	auto type() const -> int override { return 0; }

	void setUniforms(const Shader& shader, int index) const override;

private:
	glm::vec3 m_attenuation;
};

class DirectionalLight : public Light {
public:
	explicit DirectionalLight(const CS300Parser::Light& data);

	auto type() const -> int override { return 1; }

	void setUniforms(const Shader& shader, int index) const override;

private:
	glm::vec3 m_direction;
};

class SpotLight : public Light {
public:
	explicit SpotLight(const CS300Parser::Light& data);

	auto type() const -> int override { return 2; }

	void setUniforms(const Shader& shader, int index) const override;

private:
	glm::vec3 m_direction;
	glm::vec3 m_attenuation;
	float m_inner;
	float m_outer;
	float m_falloff;
};
