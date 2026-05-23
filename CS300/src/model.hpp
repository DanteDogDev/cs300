/**
 * @file model.hpp
 * @author Dante Harper
 * @date 19 May 26
 */

#pragma once

#include "mesh.hpp"
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <memory>

class Model {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

public:
	~Model() = default;

	auto getVertices() -> std::vector<Vertex>;

	static auto create(const std::string& path) -> std::unique_ptr<Model>;

private:
	Model(const std::string& filepath);
};
