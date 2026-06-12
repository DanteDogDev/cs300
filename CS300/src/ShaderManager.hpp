#pragma once

#include "Shader.hpp"

#include <map>
#include <string>

// owns the shader programs used by the app
class ShaderManager {
public:
	ShaderManager() = default;

	ShaderManager(const ShaderManager&) = delete;
	auto operator=(const ShaderManager&) -> ShaderManager& = delete;

	void load(const std::string& name, const std::string& vertex_path, const std::string& fragment_path);
	auto get(const std::string& name) const -> const Shader&;

private:
	struct {
		std::map<std::string, Shader> programs;
	} m;
};
