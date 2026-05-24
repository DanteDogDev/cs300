/**
 * @file generate.hpp
 * @author Dante Harper
 * @date 24 May 26
 */

#pragma once

#include "mesh.hpp"

#include <vector>

namespace generated {

auto generatePlane() -> std::vector<Vertex>;
auto generateCube() -> std::vector<Vertex>;
auto generateCone(int slices) -> std::vector<Vertex>;
auto generateCylinder(int slices) -> std::vector<Vertex>;
auto generateSphere(int slices, int rings) -> std::vector<Vertex>;

void init(int slices, int rings);

void regenerate(int slices, int rings);
}
