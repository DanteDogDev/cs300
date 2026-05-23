#include "model.hpp"

#include <iostream>

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
