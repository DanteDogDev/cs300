/**
 * @file mesh.hpp
 * @author Dante Harper
 * @date 12 May 26
 */

#pragma once

#include "open_gl.hpp"
#include "tiny_obj_loader.h"

#include <memory>
#include <vector>

namespace cs300 {

class Mesh {
	struct {
		std::vector<Vertex> vertices;
		std::vector<Vertex> normal_lines;
		std::vector<Vertex> averaged_normal_lines;
		gl::Mesh mesh;
		gl::Mesh normal;
		gl::Mesh normal_average;
	} m;

public:
	Mesh(const Mesh&) = delete;
	auto operator=(const Mesh&) -> Mesh& = delete;
	Mesh(Mesh&&) = delete;
	auto operator=(Mesh&&) -> Mesh& = delete;

	void make(std::vector<Vertex> verts);
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
}
