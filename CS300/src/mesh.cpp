#include "mesh.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

Mesh::Mesh(std::vector<Vertex> verts) {
	m.vertices = std::move(verts);

	glGenVertexArrays(1, &m.vao);
	glGenBuffers(1, &m.vbo);

	glBindVertexArray(m.vao);
	glBindBuffer(GL_ARRAY_BUFFER, m.vbo);

	glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Vertex), m.vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

	glBindVertexArray(0);
}

Mesh::~Mesh() {
	if (m.vao) {
		glDeleteVertexArrays(1, &m.vao);
	}
	if (m.vbo) {
		glDeleteBuffers(1, &m.vbo);
	}
}

Mesh::Mesh(Mesh&& other) noexcept : m(std::move(other.m)) {
	other.m.vao = 0;
	other.m.vbo = 0;
}

// auto Mesh::createTri() -> std::unique_ptr<Mesh> {
// 	std::vector<Vertex> verts = {
// 	  {  {0.75f, 0.75f, 0.0f}, {1.0f, 0.0f, 0.0f}},
// 	  { {0.75f, -0.75f, 0.0f}, {0.0f, 1.0f, 0.0f}},
// 	  {{-0.75f, -0.75f, 0.0f}, {0.0f, 0.0f, 1.0f}}
// 	};
// 	return std::unique_ptr<Mesh>(new Mesh(verts));
// }

auto Mesh::create(std::vector<Vertex> verts) -> std::unique_ptr<Mesh> {
	return std::unique_ptr<Mesh>(new Mesh(std::move(verts)));
}

void Mesh::draw() const {
	if (m.vao == 0) {
		return;
	}
	glBindVertexArray(m.vao);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m.vertices.size()));
	glBindVertexArray(0);
}

auto Model::getVertices() -> std::vector<Vertex> {
	std::vector<Vertex> vertices;
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex {};

			vertex.pos = {
			  attrib.vertices[(3 * index.vertex_index) + 0],
			  attrib.vertices[(3 * index.vertex_index) + 1],
			  attrib.vertices[(3 * index.vertex_index) + 2]
			};

			if (!attrib.colors.empty()) {
				vertex.color = {
				  attrib.colors[(3 * index.vertex_index) + 0],
				  attrib.colors[(3 * index.vertex_index) + 1],
				  attrib.colors[(3 * index.vertex_index) + 2]
				};
			} else {
				vertex.color = glm::vec3(1.0f);
			}

			vertices.push_back(vertex);
		}
	}
	return vertices;
}

auto Model::create(std::string_view path) -> std::unique_ptr<Model> {
	auto* ptr = new Model(path);
	return std::unique_ptr<Model>(ptr);
}

Model::Model(std::string_view filepath) {
	tinyobj::ObjReaderConfig reader_config;
	reader_config.triangulate = true;

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(std::string(filepath), reader_config)) {
		if (!reader.Error().empty()) {
			throw std::runtime_error("TinyOBJ Error: " + reader.Error());
		}
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyOBJ Warning: " << reader.Warning() << '\n';
	}

	attrib = reader.GetAttrib();
	shapes = reader.GetShapes();
	materials = reader.GetMaterials();
}
