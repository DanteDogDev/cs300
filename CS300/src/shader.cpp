#include "shader.hpp"

#include "file.hpp"

#include <memory>

Shader::Shader(std::string_view name, std::string_view vertex_path, std::string_view fragment_path) {
	m.name = std::string(name);
	m.vertex_path = std::string(vertex_path);
	m.fragment_path = std::string(fragment_path);
	compile();
}

Shader::~Shader() {
	glDeleteProgram(m.shader_program);
}

auto Shader::compileShader(const std::string& source, GLenum shader_type) -> GLint {
	GLint shader = glCreateShader(shader_type);
	const char* source_str = source.c_str();
	glShaderSource(shader, 1, &source_str, nullptr);
	glCompileShader(shader);

	// Error Checking
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

		GLchar* log_str = new GLchar[log_length + 1];
		glGetShaderInfoLog(shader, log_length, nullptr, log_str);

		const char* str_shader_type = nullptr;
		switch (shader_type) {
			case GL_VERTEX_SHADER: str_shader_type = "vertex"; break;
			case GL_GEOMETRY_SHADER: str_shader_type = "geometry"; break;
			case GL_FRAGMENT_SHADER: str_shader_type = "fragment"; break;
		}
		fprintf(stderr, "Compile failure in %s shader:\n%s\n", str_shader_type, log_str);
		delete[] log_str;
	}

	return shader;
}

void Shader::compile() {
	m.shader_program = glCreateProgram();

	// load the shaders
	auto vert = compileShader(loadFile(m.vertex_path), GL_VERTEX_SHADER);
	auto frag = compileShader(loadFile(m.fragment_path), GL_FRAGMENT_SHADER);

	// attach shaders to the program
	glAttachShader(m.shader_program, vert);
	glAttachShader(m.shader_program, frag);

	// link the program
	glLinkProgram(m.shader_program);

	// Error Checking
	GLint status;
	glGetProgramiv(m.shader_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint log_length;
		glGetProgramiv(m.shader_program, GL_INFO_LOG_LENGTH, &log_length);

		GLchar* log_str = new GLchar[log_length + 1];
		glGetProgramInfoLog(m.shader_program, log_length, nullptr, log_str);

		fprintf(stderr, "Linker failure: %s\n", log_str);
		delete[] log_str;
	}

	// clean up
	glDetachShader(m.shader_program, vert);
	glDetachShader(m.shader_program, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Shader::recompile() {
	glDeleteProgram(m.shader_program);
	compile();
}

void Shader::bind() const {
	glUseProgram(m.shader_program);
}

void Shader::unbind() const {
	glUseProgram(0);
}

auto Shader::makeDefault() -> std::shared_ptr<Shader> {
	return std::make_shared<Shader>(Shader("default", "./data/shaders/default.vert", "./data/shaders/default.frag"));
}
