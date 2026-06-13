#include "Shader.hpp"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <vector>

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
	GLuint vertex_shader = compileShader(GL_VERTEX_SHADER, readFile(vertex_path), vertex_path);
	GLuint fragment_shader = compileShader(GL_FRAGMENT_SHADER, readFile(fragment_path), fragment_path);

	m.id = glCreateProgram();
	glAttachShader(m.id, vertex_shader);
	glAttachShader(m.id, fragment_shader);
	glLinkProgram(m.id);

	GLint success;
	glGetProgramiv(m.id, GL_LINK_STATUS, &success);
	if (!success) {
		GLint log_length;
		glGetProgramiv(m.id, GL_INFO_LOG_LENGTH, &log_length);
		std::vector<char> log(log_length);
		glGetProgramInfoLog(m.id, log_length, nullptr, log.data());
		std::cerr << "Shader Link Error (" << vertex_path << " + " << fragment_path << "):\n" << log.data() << '\n';
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

auto Shader::readFile(const std::string& path) -> std::string {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Shader Load Error: could not open " << path << '\n';
		return "";
	}
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

Shader::~Shader() {
	if (m.id != 0) {
		glDeleteProgram(m.id);
	}
}

Shader::Shader(Shader&& other) noexcept {
	m.id = other.m.id;
	other.m.id = 0;
}

auto Shader::operator=(Shader&& other) noexcept -> Shader& {
	if (this != &other) {
		if (m.id != 0) {
			glDeleteProgram(m.id);
		}
		m.id = other.m.id;
		other.m.id = 0;
	}
	return *this;
}

void Shader::use() const {
	glUseProgram(m.id);
}

void Shader::setUniform(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(m.id, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(m.id, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(m.id, name.c_str()), static_cast<int>(value));
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(m.id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(m.id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

auto Shader::compileShader(GLenum type, const std::string& source, const std::string& path) -> GLuint {
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		std::vector<char> log(log_length);
		glGetShaderInfoLog(shader, log_length, nullptr, log.data());
		std::cerr << "Shader Compilation Error (" << path << "):\n" << log.data() << '\n';
	}

	return shader;
}
