#include "generate.hpp"

#include "glm/ext/scalar_constants.hpp"
#include "open_gl.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#undef TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <iostream>

auto Manager::generatePlane() -> std::vector<Vertex> {
	std::vector<Vertex> verts = {
	  {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	  { {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	  {  {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	  {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	  {  {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	  { {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
	};
	return verts;
}

auto Manager::generateCube() -> std::vector<Vertex> {
	std::vector<Vertex> face_verts;
	auto add_face = [&](glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 n) {
		face_verts.push_back({
		  p1, n, {0, 0}
		});
		face_verts.push_back({
		  p2, n, {1, 0}
		});
		face_verts.push_back({
		  p3, n, {1, 1}
		});
		face_verts.push_back({
		  p1, n, {0, 0}
		});
		face_verts.push_back({
		  p3, n, {1, 1}
		});
		face_verts.push_back({
		  p4, n, {0, 1}
		});
	};

	add_face({-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}, {0, 0, 1});
	add_face({0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}, {0, 0, -1});
	add_face({-0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}, {0, 1, 0});
	add_face({-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}, {-0.5, -0.5, 0.5}, {0, -1, 0});
	add_face({-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5}, {-0.5, 0.5, 0.5}, {-0.5, 0.5, -0.5}, {-1, 0, 0});
	add_face({0.5, -0.5, 0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}, {1, 0, 0});
	return face_verts;
}

auto Manager::generateCone(int slices) -> std::vector<Vertex> {
	std::vector<Vertex> face_verts;
	float angle_step = 2.0f * glm::pi<float>() / slices;

	for (int i = 0; i < slices; ++i) {
		float a1 = i * angle_step;
		float a2 = (i + 1) * angle_step;
		float u1 = (float)i / slices;
		float u2 = (float)(i + 1) / slices;
		float umid = (u1 + u2) * 0.5f;

		glm::vec3 p1 = {0.5f * std::cos(a1), -0.5f, 0.5f * std::sin(a1)};
		glm::vec3 p2 = {0.5f * std::cos(a2), -0.5f, 0.5f * std::sin(a2)};
		glm::vec3 center = {0, -0.5f, 0};
		glm::vec3 tip = {0, 0.5f, 0};

		glm::vec3 down = {0, -1, 0};
		face_verts.push_back({
		  center, down, {umid, 0.0f}
		});
		face_verts.push_back({
		  p1, down, {u1, 0.0f}
		});
		face_verts.push_back({
		  p2, down, {u2, 0.0f}
		});

		glm::vec3 fn = glm::normalize(glm::cross(p1 - p2, tip - p2));
		face_verts.push_back({
		  p2, fn, {u2, 0.0f}
		});
		face_verts.push_back({
		  p1, fn, {u1, 0.0f}
		});
		face_verts.push_back({
		  tip, fn, {umid, 1.0f}
		});
	}
	return face_verts;
}

auto Manager::generateCylinder(int slices) -> std::vector<Vertex> {
	std::vector<Vertex> face_verts;
	float angle_step = 2.0f * glm::pi<float>() / slices;

	auto get_pos = [&](float a, float y) { return glm::vec3(0.5f * std::cos(a), y, 0.5f * std::sin(a)); };

	for (int i = 0; i < slices; ++i) {
		float a1 = i * angle_step;
		float a2 = (i + 1) * angle_step;
		float u1 = (float)i / slices;
		float u2 = (float)(i + 1) / slices;
		float umid = (u1 + u2) * 0.5f;

		glm::vec3 t1 = get_pos(a1, 0.5f);
		glm::vec3 t2 = get_pos(a2, 0.5f);
		glm::vec3 b1 = get_pos(a1, -0.5f);
		glm::vec3 b2 = get_pos(a2, -0.5f);
		glm::vec3 top_center = {0, 0.5f, 0};
		glm::vec3 bot_center = {0, -0.5f, 0};

		glm::vec3 up = {0, 1, 0};
		face_verts.push_back({
		  top_center, up, {umid, 1.0f}
		});
		face_verts.push_back({
		  t2, up, {u2, 1.0f}
		});
		face_verts.push_back({
		  t1, up, {u1, 1.0f}
		});

		glm::vec3 down = {0, -1, 0};
		face_verts.push_back({
		  bot_center, down, {umid, 0.0f}
		});
		face_verts.push_back({
		  b1, down, {u1, 0.0f}
		});
		face_verts.push_back({
		  b2, down, {u2, 0.0f}
		});

		glm::vec3 fn1 = glm::normalize(glm::cross(b1 - b2, t1 - b2));
		glm::vec3 fn2 = glm::normalize(glm::cross(t1 - b2, t2 - b2));

		face_verts.push_back({
		  b2, fn1, {u2, 0.0f}
		});
		face_verts.push_back({
		  b1, fn1, {u1, 0.0f}
		});
		face_verts.push_back({
		  t1, fn1, {u1, 1.0f}
		});

		face_verts.push_back({
		  b2, fn2, {u2, 0.0f}
		});
		face_verts.push_back({
		  t1, fn2, {u1, 1.0f}
		});
		face_verts.push_back({
		  t2, fn2, {u2, 1.0f}
		});
	}
	return face_verts;
}

auto Manager::generateSphere(int slices, int rings) -> std::vector<Vertex> {
	std::vector<Vertex> face_verts;
	for (int i = 0; i < rings; ++i) {
		float phi1 = glm::pi<float>() * (float)i / rings;
		float phi2 = glm::pi<float>() * (float)(i + 1) / rings;
		for (int j = 0; j < slices; ++j) {
			float theta1 = 2.0f * glm::pi<float>() * (float)j / slices;
			float theta2 = 2.0f * glm::pi<float>() * (float)(j + 1) / slices;

			auto get_pos = [&](float p, float t) {
				return glm::vec3(0.5f * std::sin(p) * std::cos(t), 0.5f * std::cos(p), 0.5f * std::sin(p) * std::sin(t));
			};
			auto get_uv = [&](float p, float t) { return glm::vec2(t / (2.0f * glm::pi<float>()), 1.0f - (p / glm::pi<float>())); };

			glm::vec3 p1 = get_pos(phi1, theta1);
			glm::vec3 p2 = get_pos(phi1, theta2);
			glm::vec3 p3 = get_pos(phi2, theta1);
			glm::vec3 p4 = get_pos(phi2, theta2);

			glm::vec2 uv1 = get_uv(phi1, theta1);
			glm::vec2 uv2 = get_uv(phi1, theta2);
			glm::vec2 uv3 = get_uv(phi2, theta1);
			glm::vec2 uv4 = get_uv(phi2, theta2);

			auto add_tri = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 uva, glm::vec2 uvb, glm::vec2 uvc) {
				glm::vec3 fn = glm::normalize(glm::cross(b - a, c - a));
				face_verts.push_back({a, fn, uva});
				face_verts.push_back({b, fn, uvb});
				face_verts.push_back({c, fn, uvc});
			};

			if (i != rings - 1) {
				add_tri(p4, p3, p1, uv4, uv3, uv1);
			}
			if (i != 0) {
				add_tri(p4, p1, p2, uv4, uv1, uv2);
			}
		}
	}
	return face_verts;
}

auto Manager::getMesh(const std::string& name) -> cs300::Mesh* {
	auto it = meshes.find(name);
	if (it == meshes.end()) {
		addMesh(name);
		it = meshes.find(name);
	}
	return it->second.get();
}

auto Manager::getShader(const std::string& name) -> gl::Shader* {
	auto it = shaders.find(name);
	if (it == shaders.end()) {
		std::cerr << "Shader Not Found " << name << "\n";
		return nullptr;
	}
	return it->second.get();
}

auto Manager::getTexture(const std::string& name) -> gl::Texture* {
	auto it = textures.find(name);
	if (it == textures.end()) {
		addTexture(name);
		it = textures.find(name);
	}
	return it->second.get();
}

void Manager::addMesh(const std::string& path) {
	std::cout << "Adding Mesh: " << path << "\n";
	cs300::Model model = cs300::Model(path);
	auto mesh = cs300::Mesh::create(model.getVertices());
	meshes[path] = std::move(mesh);
}

void Manager::addShader(const std::string& name, const std::string& vert, const std::string& frag) {
	std::cout << "Adding Shader: " << name << "\n";
	auto* shader = new gl::Shader(vert, frag);
	auto ptr = std::unique_ptr<gl::Shader>(shader);
	shaders[name] = std::move(ptr);
}

void Manager::addTexture(const std::string& path) {
	std::cout << "Adding Texture: " << path << "\n";
	auto* texture = new gl::Texture(path);
	auto ptr = std::unique_ptr<gl::Texture>(texture);
	textures[path] = std::move(ptr);
}

void Manager::init(int slices, int rings) {
	std::cout << "Generating Meshes\n";
	addShader("default", "./data/shaders/default.vert", "./data/shaders/default.frag");
	meshes["PLANE"] = cs300::Mesh::create(generatePlane());
	meshes["CUBE"] = cs300::Mesh::create(generateCube());
	meshes["CONE"] = cs300::Mesh::create(generateCone(slices));
	meshes["CYLINDER"] = cs300::Mesh::create(generateCylinder(slices));
	meshes["SPHERE"] = cs300::Mesh::create(generateSphere(slices, rings));
}

void Manager::regenerate(int slices, int rings) {
	std::cout << "Regenerating Meshes\n";
	getMesh("CONE")->make(generateCone(slices));
	getMesh("CYLINDER")->make(generateCylinder(slices));
	getMesh("SPHERE")->make(generateSphere(slices, rings));
}

void Manager::clear() {
	meshes.clear();
	shaders.clear();
	textures.clear();
}
