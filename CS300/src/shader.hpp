/**
 * @file shader.hpp
 * @author Dante Harper
 * @date 07 May 26
 *
 * @brief Opengl Mesh Abstraction
 * @todo support more than just vertex and fragment?
 */

#pragma once
#include <memory>
#include <string>

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

class Shader {
	struct {
		std::string vertex_path;
		std::string fragment_path;
		GLint shader_program;
	} m;

public:
	~Shader();

	/* Binds shader :3*/
	void bind() const;

	/* Unbinds shader :(*/
	void unbind() const;

	/* Creates a Default Shader */
	static auto createDefault() -> std::unique_ptr<Shader>;

	static auto create(std::string_view vertex_path, std::string_view fragment_path) -> std::unique_ptr<Shader>;
	[[nodiscard]]
	auto getProgram() const {
		return m.shader_program;
	}

private:
	Shader(std::string_view vertex_path, std::string_view fragment_path);
	auto compileShader(const std::string& source, GLenum shader_type) -> GLint;

	/* Compiles the while shader program together*/
	void compile();

	/* Recompiles the shader program */
	void recompile();
};
