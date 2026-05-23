#include "resources.hpp"

auto ResourceManager::instance() -> ResourceManager& {
	static ResourceManager inst;
	return inst;
}

void ResourceManager::clear() {
	meshes.clear();
	shaders.clear();
}

auto ResourceManager::getMesh(const std::string& path) -> Mesh* {
	auto it = meshes.find(path);
	if (it != meshes.end()) {
		return it->second.get();
	}
	auto model = Model(path);
	auto mesh = Mesh::create(model.getVertices());
	Mesh* ptr = mesh.get();
	meshes[path] = std::move(mesh);
	return ptr;
}

auto ResourceManager::getShader(const std::string& name, const std::string& vert, const std::string& frag) -> Shader* {
	auto it = shaders.find(name);
	if (it != shaders.end()) {
		return it->second.get();
	}
	if (vert.empty() || frag.empty()) {
		return nullptr;
	}
	auto shader = Shader::create(vert, frag);
	Shader* ptr = shader.get();
	shaders[name] = std::move(shader);
	return ptr;
}
