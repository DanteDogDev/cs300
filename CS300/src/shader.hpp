/**
 * @file shader.hpp
 * @author Dante Harper
 * @date 07 May 26
 *
 * @brief Opengl Mesh Abstraction
 * @todo support more than just vertex and fragment?
 */

#pragma once
#include "open_gl.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <string>

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

namespace cs300 {

class Shader {
	struct {
		std::string vertex_path;
		std::string fragment_path;
		gl::Shader shader;
	} m;

public:
	void bind() const;

	void unbind() const;

	void setUniformMat4(const std::string& name, const glm::mat4& matrix) const;
	void setUniform1i(const std::string& name, int value) const;

	static auto createDefault() -> std::unique_ptr<Shader>;

	static auto create(std::string_view vertex_path, std::string_view fragment_path) -> std::unique_ptr<Shader>;


private:
	Shader(std::string_view vertex_path, std::string_view fragment_path);
	auto compileShader(const std::string& source, GLenum shader_type) -> GLint;

	void compile();

	void recompile();
};
}
