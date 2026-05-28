#include "shader.hpp"

#include "file.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>

namespace cs300 {

Shader::Shader(std::string_view vertex_path, std::string_view fragment_path) {
	m.vertex_path = std::string(vertex_path);
	m.fragment_path = std::string(fragment_path);
	compile();
}

void Shader::setUniform1i(const std::string& name, int value) const {
	m.shader.setUniform(name, value);
}

void Shader::setUniformMat4(const std::string& name, const glm::mat4& matrix) const {
	m.shader.setUniform(name, matrix);
}

void Shader::compile() {
	m.shader.make(m.vertex_path, m.fragment_path);
}

void Shader::recompile() {
	m.shader.make(m.vertex_path, m.fragment_path);
}

void Shader::bind() const {
	m.shader.bind();
}

void Shader::unbind() const {
	m.shader.unbind();
}

auto Shader::createDefault() -> std::unique_ptr<Shader> {
	auto* ptr = new Shader("./data/shaders/default.vert", "./data/shaders/default.frag");
	return std::unique_ptr<Shader>(ptr);
}

auto Shader::create(std::string_view vertex_path, std::string_view fragment_path) -> std::unique_ptr<Shader> {
	auto* ptr = new Shader(vertex_path, fragment_path);
	return std::unique_ptr<Shader>(ptr);
}
}
