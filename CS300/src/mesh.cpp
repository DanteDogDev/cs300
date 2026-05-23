#include "mesh.hpp"

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
