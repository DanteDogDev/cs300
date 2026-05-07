#pragma once
#include <memory>
#include <string>

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

class Shader {
	struct {
		std::string name;
		std::string vertex_path;
		std::string fragment_path;
		GLint shader_program;
	} m;

public:
	Shader(std::string_view name, std::string_view vertex_path, std::string_view fragment_path);

	~Shader();

	/* Binds shader :3*/
	void bind() const;

	/* Unbinds shader :(*/
	void unbind() const;

	/* Creates a Default Shader */
	static auto makeDefault() -> Shader;

	[[nodiscard]] auto getProgram() const { return m.shader_program; }

private:
	auto compileShader(const std::string& source, GLenum shader_type) -> GLint;

	/* Compiles the while shader program together*/
	void compile();

	/* Recompiles the shader program */
	void recompile();
};
