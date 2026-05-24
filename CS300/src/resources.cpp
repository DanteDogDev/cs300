#include "resources.hpp"

#include "glm/common.hpp"
#include "mesh.hpp"
#include "texture.hpp"

#include <iostream>    // For logging errors
#include <utility>

auto ResourceManager::instance() -> ResourceManager& {
	static ResourceManager inst;
	return inst;
}

void ResourceManager::clear() {
	meshes.clear();
	shaders.clear();
}

void ResourceManager::init() {
	std::vector<Vertex> default_verts = {
	  {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	  { {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	  {  {0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
	};
	addMesh("default", default_verts);
	addShader("default", "./data/shaders/default.vert", "./data/shaders/default.frag");
	addTexture("default", new Texture());
}

auto ResourceManager::getShader(const std::string& name) -> Shader* {
	if (shaders.find(name) != shaders.end()) {
		return shaders["default"].get();
	}
	return shaders[name].get();
}

auto ResourceManager::getMesh(const std::string& path) -> Mesh* {
	if (meshes.find(path) == meshes.end()) {
		try {
			addMesh(path);
		} catch (...) { return meshes["default"].get(); }
	}
	return meshes[path].get();
}

auto ResourceManager::getTexture(const std::string& name) -> Texture* {
	if (textures.find(name) != textures.end()) {
		return textures["default"].get();
	}
	return textures[name].get();
}

void ResourceManager::addShader(const std::string& name, const std::string& vert, const std::string& frag) {
	auto shader = Shader::create(vert, frag);
	Shader* ptr = shader.get();
	shaders[name] = std::move(shader);
}

void ResourceManager::addMesh(const std::string& path) {
	Model model = Model(path);
	auto mesh = Mesh::create(model.getVertices());
	meshes[path] = std::move(mesh);
}

void ResourceManager::addMesh(const std::string& name, std::vector<Vertex> vertices) {
	auto mesh = Mesh::create(std::move(vertices));
	meshes[name] = std::move(mesh);
}

void ResourceManager::addTexture(const std::string& name, Texture* tex) {
	textures["name"] = std::unique_ptr<Texture>(tex);
}
