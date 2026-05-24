/**
 * @file mesh.hpp
 * @author Dante Harper
 * @date 12 May 26
 */

#pragma once

#include "cs300/OGLDebug.h"
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Mesh {
	struct {
		std::vector<Vertex> vertices;
		std::vector<Vertex> normal_lines;
		std::vector<Vertex> averaged_normal_lines;
	} m;

	struct {
		GLuint vbo = 0;
		GLuint vao = 0;
	} mesh;

	struct {
		GLuint vbo = 0;
		GLuint vao = 0;
	} normal;

	struct {
		GLuint vbo = 0;
		GLuint vao = 0;
	} normal_average;

public:
	~Mesh();

	Mesh(const Mesh&) = delete;
	auto operator=(const Mesh&) -> Mesh& = delete;

	void remake(std::vector<Vertex> verts);
	static auto create(std::vector<Vertex> verts) -> std::unique_ptr<Mesh>;

	void draw() const;
	void drawNormals() const;

	void drawAveragedNormals() const;

private:
	Mesh(std::vector<Vertex> verts);

	void generateNormalLines();

	void generateAveragedNormals();
};

class Model {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

public:
	Model(std::string_view filepath);
	~Model() = default;

	auto getVertices() -> std::vector<Vertex>;

	static auto create(std::string_view path) -> std::unique_ptr<Model>;

private:
};
