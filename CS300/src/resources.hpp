#include "mesh.hpp"
#include "model.hpp"
#include "shader.hpp"

#include <memory>
#include <unordered_map>

class Resources {
	static std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
	static std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;

public:
	static auto clear() {
		meshes.clear();
		shaders.clear();
	}

	static auto getMesh(std::string_view path) -> Mesh* {
		std::string copy_path(path);
		auto it = meshes.find(copy_path);

		if (it != meshes.end()) {
			return it->second.get();
		}

		auto model = Model(path);
		auto mesh = Mesh::create(model.getVertices());
		Mesh* raw_ptr = mesh.get();

		meshes[copy_path] = std::move(mesh);
		return raw_ptr;
	}

	static auto getShader(std::string_view name) -> Shader* {
		std::string copy_name(name);
		auto it = shaders.find(copy_name);

		if (it != shaders.end()) {
			return it->second.get();
		}

		// Fallback to "default" if it exists
		auto default_it = shaders.find("default");
		if (default_it != shaders.end()) {
			return default_it->second.get();
		}

		return nullptr;
	}

	static auto addShader(std::string_view name, std::string_view vertex_path, std::string_view fragment_path) -> Shader* {
		std::string copy_name(name);
		auto it = shaders.find(copy_name);

		if (it != shaders.end()) {
			return it->second.get();
		}

		auto shader = Shader::create(vertex_path, fragment_path);
		Shader* raw_ptr = shader.get();

		shaders[copy_name] = std::move(shader);
		return raw_ptr;
	}
};
