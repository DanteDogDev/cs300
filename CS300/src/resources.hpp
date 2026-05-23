/**
 * @file resources.hpp
 * @author Dante Harper
 * @date 23 May 26
 *
 * @brief [TODO: Brief description of the file's purpose]
 */

#pragma once

#include "mesh.hpp"
#include "shader.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager {
	ResourceManager() = default;
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
	std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;

public:
	static auto instance() -> ResourceManager&;
	void init();    // New: Initialize default resources
	void clear();

	auto getMesh(const std::string& path) -> Mesh*;
	auto getShader(const std::string& name) -> Shader*;


	void addShader(const std::string& name, const std::string& vert = "", const std::string& frag = "");
	void addMesh(const std::string& path);
	void addMesh(const std::string& name, std::vector<Vertex>& vertices);
};
