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

// struct Texture {
// 	Texture() = default;
// 	~Texture() {
// 		if (texture_id) {
// 			glDeleteTextures(1, &texture_id);
// 		}
// 	}
//
// 	Texture(const Texture&) = delete;
// 	Texture(Texture&&) = delete;
// 	auto operator=(const Texture&) -> Texture& = delete;
// 	auto operator=(Texture&&) -> Texture& = delete;
//
// 	GLuint texture_id = 0;
// 	int width = 0;
// 	int height = 0;
// 	int channels = 0;
//
//
// 	void make(const unsigned char* pixels, int w, int h) {
// 		if (texture_id) {
// 			glDeleteTextures(1, &texture_id);
// 			texture_id = 0;
// 		}
//
// 		width = w;
// 		height = h;
//
// 		glGenTextures(1, &texture_id);
// 		glBindTexture(GL_TEXTURE_2D, texture_id);
//
// 		// Default wrapping parameters
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
// 		// Default filtering parameters with Mipmaps
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
// 		// Upload raw RGBA buffer to GPU
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
// 		glGenerateMipmap(GL_TEXTURE_2D);
//
// 		glBindTexture(GL_TEXTURE_2D, 0);
// 	}
//
// 	void bind(unsigned int slot) const {
// 		// GL_TEXTURE0 is guaranteed to be a sequential enum macro
// 		glActiveTexture(GL_TEXTURE0 + slot);
// 		glBindTexture(GL_TEXTURE_2D, texture_id);
// 	}
//
// 	void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
// };
}
