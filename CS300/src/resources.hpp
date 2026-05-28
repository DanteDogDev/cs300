/**
 * @file resources.hpp
 * @author Dante Harper
 * @date 23 May 26
 */

#pragma once

#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager {
	ResourceManager() = default;
	std::unordered_map<std::string, std::unique_ptr<cs300::Mesh>> meshes;
	std::unordered_map<std::string, std::unique_ptr<cs300::Shader>> shaders;
	std::unordered_map<std::string, std::unique_ptr<cs300::Texture>> textures;

public:
	static auto instance() -> ResourceManager&;
	void init();    // New: Initialize default resources
	void clear();

	auto getMesh(const std::string& path) -> cs300::Mesh*;
	auto getShader(const std::string& name) -> cs300::Shader*;

	auto getTexture(const std::string& name) -> cs300::Texture*;

	void addTexture(const std::string& name, cs300::Texture* tex);
	void addShader(const std::string& name, const std::string& vert = "", const std::string& frag = "");
	void addMesh(const std::string& path);
	void addMesh(const std::string& name, std::vector<Vertex> vertices);
};
