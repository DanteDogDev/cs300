/**
 * @file model.hpp
 * @author Dante Harper
 * @date 19 May 26
 */

#pragma once

#include "mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#undef TINYOBJLOADER_IMPLEMENTATION

#include <glm/glm.hpp>
#include <iostream>
#include <memory>

class Model {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

public:
	~Model() = default;

	auto getVertices() -> std::vector<Vertex> {
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

	static auto create(const std::string& path) -> std::unique_ptr<Model> {
		auto* ptr = new Model(path);
		return std::unique_ptr<Model>(ptr);
	}

private:
	Model(const std::string& filepath) {
		tinyobj::ObjReaderConfig reader_config;
		reader_config.triangulate = true;

		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(filepath, reader_config)) {
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
};
