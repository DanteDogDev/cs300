/**
 * @file generate.hpp
 * @author Dante Harper
 * @date 24 May 26
 */

#pragma once

#include "mesh.hpp"
#include "open_gl.hpp"

#include <map>
#include <memory>
#include <vector>

class Manager {
	static inline std::map<std::string, std::unique_ptr<cs300::Mesh>> meshes;
	static inline std::map<std::string, std::unique_ptr<gl::Shader>> shaders;
	static inline std::map<std::string, std::unique_ptr<gl::Texture>> textures;

public:
	static auto getMesh(const std::string& name) -> cs300::Mesh*;
	static auto getShader(const std::string& name) -> gl::Shader*;
	static auto getTexture(const std::string& name) -> gl::Texture*;
	static void addMesh(const std::string& path);
	static void addShader(const std::string& name, const std::string& vert, const std::string& frag);
	static void addTexture(const std::string& path);
	static void init(int slices, int rings);
	static void regenerate(int slices, int rings);
	static void clear();

private:
	static auto generatePlane() -> std::vector<Vertex>;
	static auto generateCube() -> std::vector<Vertex>;
	static auto generateCone(int slices) -> std::vector<Vertex>;
	static auto generateCylinder(int slices) -> std::vector<Vertex>;
	static auto generateSphere(int slices, int rings) -> std::vector<Vertex>;
};
