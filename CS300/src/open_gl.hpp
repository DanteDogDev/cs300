/**
 * @file open_gl.hpp
 * @author Dante Harper
 * @date 28 May 26
 *
 * @brief simplest opengl abstractions
 */

#pragma once

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

#include <glm/glm.hpp>
#include <iostream>
#include <stb/stb_image.h>
#include <string>
#include <vector>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

namespace gl {

struct Mesh {
	Mesh() = default;
	Mesh(const std::vector<Vertex>& verts);

	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	auto operator=(const Mesh&) -> Mesh& = delete;
	auto operator=(Mesh&&) -> Mesh& = delete;

	GLsizei size = 0;
	GLuint vbo = 0;
	GLuint vao = 0;

	void make(const std::vector<Vertex>& verts);

	void drawTriangles() const;
	void drawLines() const;
};

struct Shader {
	Shader() = default;
	Shader(std::string_view vertex_path, std::string_view fragment_path);
	~Shader();

	Shader(const Shader&) = delete;
	Shader(Shader&&) = delete;
	auto operator=(const Shader&) -> Shader& = delete;
	auto operator=(Shader&&) -> Shader& = delete;

	GLint shader_program = 0;

	void make(std::string_view vertex_path, std::string_view fragment_path);

	void bind() const;
	void unbind() const;

	void setUniform(const std::string& name, bool value) const;
	void setUniform(const std::string& name, int value) const;
	void setUniform(const std::string& name, float value) const;
	void setUniform(const std::string& name, const glm::vec2& value) const;
	void setUniform(const std::string& name, const glm::vec3& value) const;
	void setUniform(const std::string& name, const glm::vec4& value) const;
	void setUniform(const std::string& name, const glm::mat3& matrix) const;
	void setUniform(const std::string& name, const glm::mat4& matrix) const;

private:
	[[nodiscard]]
	auto getUniformLoc(const std::string& name) const -> GLint;
	auto compileShader(const std::string& source, GLenum shader_type) -> GLint;
};

struct Texture {
	Texture() = default;
	Texture(std::string_view file_path);
	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&&) = delete;
	auto operator=(const Texture&) -> Texture& = delete;
	auto operator=(Texture&&) -> Texture& = delete;

	GLuint texture_id = 0;

	void make(std::string_view file_path);

	void bind(unsigned int slot) const;

	void unbind() const;
};
}
