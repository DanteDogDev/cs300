#include "mesh.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

Mesh::Mesh(std::vector<Vertex> verts) {
	m.vertices = std::move(verts);
	generateNormalLines();
	generateAveragedNormals();

	glGenVertexArrays(1, &mesh.vao);
	glGenBuffers(1, &mesh.vbo);
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Vertex), m.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
	glBindVertexArray(0);

	glGenVertexArrays(1, &normal.vao);
	glGenBuffers(1, &normal.vbo);
	glBindVertexArray(normal.vao);
	glBindBuffer(GL_ARRAY_BUFFER, normal.vbo);
	glBufferData(GL_ARRAY_BUFFER, m.normal_lines.size() * sizeof(Vertex), m.normal_lines.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
	glBindVertexArray(0);

	glGenVertexArrays(1, &normal_average.vao);
	glGenBuffers(1, &normal_average.vbo);
	glBindVertexArray(normal_average.vao);
	glBindBuffer(GL_ARRAY_BUFFER, normal_average.vbo);
	glBufferData(GL_ARRAY_BUFFER, m.averaged_normal_lines.size() * sizeof(Vertex), m.averaged_normal_lines.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
	glBindVertexArray(0);
}

Mesh::~Mesh() {
	if (mesh.vao) {
		glDeleteVertexArrays(1, &mesh.vao);
	}
	if (mesh.vbo) {
		glDeleteBuffers(1, &mesh.vbo);
	}
	if (normal.vao) {
		glDeleteVertexArrays(1, &normal.vao);
	}
	if (normal.vbo) {
		glDeleteBuffers(1, &normal.vbo);
	}
	if (normal_average.vao) {
		glDeleteVertexArrays(1, &normal_average.vao);
	}
	if (normal_average.vbo) {
		glDeleteBuffers(1, &normal_average.vbo);
	}
}

void Mesh::remake(std::vector<Vertex> verts) {
	m.vertices = std::move(verts);
	generateNormalLines();
	generateAveragedNormals();

	if (mesh.vbo && mesh.vao) {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Vertex), m.vertices.data(), GL_STATIC_DRAW);
	}
	if (normal.vbo && normal.vao) {
		glBindBuffer(GL_ARRAY_BUFFER, normal.vbo);
		glBufferData(GL_ARRAY_BUFFER, m.normal_lines.size() * sizeof(Vertex), m.normal_lines.data(), GL_STATIC_DRAW);
	}
	if (normal_average.vbo && normal_average.vao) {
		glBindBuffer(GL_ARRAY_BUFFER, normal_average.vbo);
		glBufferData(
		    GL_ARRAY_BUFFER, m.averaged_normal_lines.size() * sizeof(Vertex), m.averaged_normal_lines.data(), GL_STATIC_DRAW
		);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

auto Mesh::create(std::vector<Vertex> verts) -> std::unique_ptr<Mesh> {
	return std::unique_ptr<Mesh>(new Mesh(std::move(verts)));
}

void Mesh::draw() const {
	if (mesh.vao == 0) {
		return;
	}
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m.vertices.size()));
	glBindVertexArray(0);
}

void Mesh::drawNormals() const {
	if (normal.vao == 0 || m.normal_lines.empty()) {
		return;
	}
	glBindVertexArray(normal.vao);
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m.normal_lines.size()));
	glBindVertexArray(0);
}

void Mesh::drawAveragedNormals() const {
	if (normal_average.vao == 0 || m.averaged_normal_lines.empty()) {
		return;
	}
	glBindVertexArray(normal_average.vao);
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m.averaged_normal_lines.size()));
	glBindVertexArray(0);
}

void Mesh::generateNormalLines() {
	m.normal_lines.clear();
	for (const auto& v : m.vertices) {
		Vertex start_vertex = v;

		Vertex end_vertex = v;
		end_vertex.pos = v.pos + v.normal * 0.5f;

		m.normal_lines.push_back(start_vertex);
		m.normal_lines.push_back(end_vertex);
	}
}

void Mesh::generateAveragedNormals() {
	m.averaged_normal_lines.clear();
	if (m.vertices.empty()) {
		return;
	}

	auto comp = [](const glm::vec3& a, const glm::vec3& b) {
		const float epsilon = 0.0001f;
		if (std::abs(a.x - b.x) > epsilon) {
			return a.x < b.x;
		}
		if (std::abs(a.y - b.y) > epsilon) {
			return a.y < b.y;
		}
		if (std::abs(a.z - b.z) > epsilon) {
			return a.z < b.z;
		}
		return false;
	};

	std::map<glm::vec3, glm::vec3, decltype(comp)> position_to_normal_map(comp);
	for (const auto& v : m.vertices) {
		position_to_normal_map[v.pos] += v.normal;
	}

	for (const auto& [pos, accumulated_normal] : position_to_normal_map) {
		glm::vec3 averaged_normal = glm::normalize(accumulated_normal);

		Vertex start_vertex {};
		start_vertex.pos = pos;
		start_vertex.normal = averaged_normal;

		Vertex end_vertex {};
		end_vertex.pos = pos + averaged_normal * 0.5f;
		end_vertex.normal = averaged_normal;

		m.averaged_normal_lines.push_back(start_vertex);
		m.averaged_normal_lines.push_back(end_vertex);
	}
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

			if (index.normal_index >= 0 && !attrib.normals.empty()) {
				vertex.normal = {
				  attrib.normals[(3 * index.normal_index) + 0],
				  attrib.normals[(3 * index.normal_index) + 1],
				  attrib.normals[(3 * index.normal_index) + 2]
				};
			} else {
				vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			}

			if (index.texcoord_index >= 0 && !attrib.texcoords.empty()) {
				vertex.uv = {attrib.texcoords[(2 * index.texcoord_index) + 0], attrib.texcoords[(2 * index.texcoord_index) + 1]};
			} else {
				vertex.uv = glm::vec2(0.0f, 0.0f);
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
