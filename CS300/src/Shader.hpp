#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
	Shader(const std::string& vertex_path, const std::string& fragment_path);
	~Shader();

	Shader(const Shader&) = delete;
	auto operator=(const Shader&) -> Shader& = delete;

	Shader(Shader&& other) noexcept;
	auto operator=(Shader&& other) noexcept -> Shader&;

	void use() const;

	void setUniform(const std::string& name, float value) const;
	void setUniform(const std::string& name, int value) const;
	void setUniform(const std::string& name, bool value) const;
	void setUniform(const std::string& name, const glm::vec3& value) const;
	void setUniform(const std::string& name, const glm::mat4& value) const;

private:
	struct {
		GLuint id = 0;
	} m;

	static auto readFile(const std::string& path) -> std::string;
	static auto compileShader(GLenum type, const std::string& source, const std::string& path) -> GLuint;
};
