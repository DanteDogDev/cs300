#include "mesh.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cs300 {

Mesh::Mesh(std::vector<Vertex> verts) {
	make(std::move(verts));
}

void Mesh::make(std::vector<Vertex> verts) {
	m.vertices = std::move(verts);
	generateNormalLines();
	generateAveragedNormals();

	m.mesh.make(m.vertices);
	m.normal.make(m.normal_lines);
	m.normal_average.make(m.averaged_normal_lines);
}

auto Mesh::create(std::vector<Vertex> verts) -> std::unique_ptr<Mesh> {
	return std::unique_ptr<Mesh>(new Mesh(std::move(verts)));
}

void Mesh::draw() const {
	m.mesh.drawTriangles();
}

void Mesh::drawNormals() const {
	m.normal.drawLines();
}

void Mesh::drawAveragedNormals() const {
	m.normal_average.drawLines();
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
	std::map<glm::vec3, glm::vec2, decltype(comp)> position_to_uv_map(comp);

	for (const auto& v : m.vertices) {
		position_to_normal_map[v.pos] += v.normal;
		position_to_uv_map[v.pos] = v.uv;
	}

	for (const auto& [pos, accumulated_normal] : position_to_normal_map) {
		glm::vec3 averaged_normal = glm::normalize(accumulated_normal);
		glm::vec2 shared_uv = position_to_uv_map[pos];

		Vertex start_vertex {};
		start_vertex.pos = pos;
		start_vertex.normal = averaged_normal;
		start_vertex.uv = shared_uv;

		Vertex end_vertex {};
		end_vertex.pos = pos + averaged_normal * 0.5f;
		end_vertex.normal = averaged_normal;
		end_vertex.uv = shared_uv;

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
}
