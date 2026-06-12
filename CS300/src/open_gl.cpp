#include "open_gl.hpp"

#include "file.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace gl {
// ---------- MESH ---------- //
Mesh::Mesh(const std::vector<Vertex>& verts) {
	make(verts);
}

Mesh::~Mesh() {
	if (vao) {
		glDeleteVertexArrays(1, &vao);
	}
	if (vbo) {
		glDeleteBuffers(1, &vbo);
	}
}

void Mesh::make(const std::vector<Vertex>& verts) {
	if (vbo && vao) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

		size = verts.size();
		return;
	}
	// Generate Buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// Bind Data
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

	// UV
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
	glBindVertexArray(0);

	size = verts.size();
}

void Mesh::drawTriangles() const {
	if (vao == 0) {
		return;
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, size);
	glBindVertexArray(0);
}

void Mesh::drawLines() const {
	if (vao == 0) {
		return;
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, size);
	glBindVertexArray(0);
}

// ---------- SHADER ---------- //
Shader::Shader(std::string_view vertex_path, std::string_view fragment_path) {
	make(vertex_path, fragment_path);
}

Shader::~Shader() {
	if (shader_program) {
		glDeleteProgram(shader_program);
	}
}

void Shader::make(std::string_view vertex_path, std::string_view fragment_path) {
	if (shader_program) {
		glDeleteProgram(shader_program);
	}
	shader_program = glCreateProgram();

	// load the shaders
	auto vert = compileShader(loadFile(vertex_path), GL_VERTEX_SHADER);
	auto frag = compileShader(loadFile(fragment_path), GL_FRAGMENT_SHADER);

	// attach shaders to the program
	glAttachShader(shader_program, vert);
	glAttachShader(shader_program, frag);

	// link the program
	glLinkProgram(shader_program);

	// Error Checking
	GLint status;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint log_length;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);

		GLchar* log_str = new GLchar[log_length + 1];
		glGetProgramInfoLog(shader_program, log_length, nullptr, log_str);

		fprintf(stderr, "Linker failure: %s\n", log_str);
		delete[] log_str;
	}

	// clean up
	glDetachShader(shader_program, vert);
	glDetachShader(shader_program, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Shader::bind() const {
	glUseProgram(shader_program);
}

void Shader::unbind() const {
	glUseProgram(0);
}

void Shader::setUniform(const std::string& name, bool value) const {
	glUniform1i(getUniformLoc(name), (int)value);
}

void Shader::setUniform(const std::string& name, int value) const {
	glUniform1i(getUniformLoc(name), value);
}

void Shader::setUniform(const std::string& name, float value) const {
	glUniform1f(getUniformLoc(name), value);
}

void Shader::setUniform(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(getUniformLoc(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(getUniformLoc(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(getUniformLoc(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::mat3& matrix) const {
	glUniformMatrix3fv(getUniformLoc(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniform(const std::string& name, const glm::mat4& matrix) const {
	glUniformMatrix4fv(getUniformLoc(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

[[nodiscard]]
auto Shader::getUniformLoc(const std::string& name) const -> GLint {
	GLint location = glGetUniformLocation(shader_program, name.c_str());
	if (location == -1) {
		std::cerr << "Uniform " << name << " Not found\n";
	}
	return location;
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

// ---------- TEXTURE ---------- //
Texture::Texture(std::string_view file_path) {
	make(file_path);
}

Texture::~Texture() {
	if (texture_id) {
		glDeleteTextures(1, &texture_id);
	}
}

void Texture::make(std::string_view file_path) {
	if (texture_id) {
		glDeleteTextures(1, &texture_id);
		texture_id = 0;
	}

	int width = 0;
	int height = 0;
	int channels = 0;

	unsigned char* data = stbi_load(file_path.data(), &width, &height, &channels, 0);

	if (!data) {
		std::cerr << "Failed To Load File " << file_path << "\n";
		return;
	}

	GLenum internal_format = 0;
	GLenum data_format = 0;
	if (channels == 1) {
		internal_format = GL_RED;
		data_format = GL_RED;
	} else if (channels == 3) {
		internal_format = GL_RGB8;
		data_format = GL_RGB;
	} else if (channels == 4) {
		internal_format = GL_RGBA8;
		data_format = GL_RGBA;
	}

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind Texture Data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

void Texture::bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}
}
