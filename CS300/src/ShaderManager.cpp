#include "ShaderManager.hpp"

#include <tuple>

void ShaderManager::load(const std::string& name, const std::string& vertex_path, const std::string& fragment_path) {
	// Shader is move only with no default ctor so build it in place
	m.programs.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(vertex_path, fragment_path));
}

auto ShaderManager::get(const std::string& name) const -> const Shader& {
	return m.programs.at(name);
}
