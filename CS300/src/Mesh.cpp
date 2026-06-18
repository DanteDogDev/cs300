#include "Mesh.hpp"

#include "glm/geometric.hpp"

#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "cs300/tiny_obj_loader.h"

#include <array>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

Mesh::Mesh(Type type, int slices, int rings) {
	switch (type) {
		case Type::plane: generatePlane(); break;
		case Type::cube: generateCube(); break;
		case Type::cone: generateCone(slices); break;
		case Type::cylinder: generateCylinder(slices); break;
		case Type::sphere: generateSphere(slices, rings); break;
		default: break;
	}
}

Mesh::Mesh(const std::string& filename) {
	loadObj(filename);
}

Mesh::~Mesh() {
	free();
}

auto Mesh::free() -> void {
	std::array<GLuint, 2> vaos = {m.face.mesh_vao, m.averaged.mesh_vao};
	std::array<GLuint, 2> vbos = {m.face.mesh_vbo, m.averaged.mesh_vbo};
	glDeleteVertexArrays(2, vaos.data());
	glDeleteBuffers(2, vbos.data());

	std::array<GLuint, 2> normal_vaos = {m.face.normal_vao, m.averaged.normal_vao};
	std::array<GLuint, 2> normal_vbos = {m.face.normal_vbo, m.averaged.normal_vbo};
	glDeleteVertexArrays(2, normal_vaos.data());
	glDeleteBuffers(2, normal_vbos.data());

	std::array<GLuint, 2> tangent_vaos = {m.face.tangent_vao, m.averaged.tangent_vao};
	std::array<GLuint, 2> tangent_vbos = {m.face.tangent_vbo, m.averaged.tangent_vbo};
	glDeleteVertexArrays(2, tangent_vaos.data());
	glDeleteBuffers(2, tangent_vbos.data());

	std::array<GLuint, 2> bitangent_vaos = {m.face.bitangent_vao, m.averaged.bitangent_vao};
	std::array<GLuint, 2> bitangent_vbos = {m.face.bitangent_vbo, m.averaged.bitangent_vbo};
	glDeleteVertexArrays(2, bitangent_vaos.data());
	glDeleteBuffers(2, bitangent_vbos.data());
}

void Mesh::draw(bool average) const {
	glBindVertexArray(average ? m.averaged.mesh_vao : m.face.mesh_vao);
	glDrawArrays(GL_TRIANGLES, 0, average ? m.averaged.mesh_size : m.face.mesh_size);
}

void Mesh::drawNormals(bool average) const {
	glBindVertexArray(average ? m.averaged.normal_vao : m.face.normal_vao);
	glDrawArrays(GL_LINES, 0, average ? m.averaged.normal_size : m.face.normal_size);
}

void Mesh::drawTangents(bool average) const {
	glBindVertexArray(average ? m.averaged.tangent_vao : m.face.tangent_vao);
	glDrawArrays(GL_LINES, 0, average ? m.averaged.tangent_size : m.face.tangent_size);
}

void Mesh::drawBitangents(bool average) const {
	glBindVertexArray(average ? m.averaged.bitangent_vao : m.face.bitangent_vao);
	glDrawArrays(GL_LINES, 0, average ? m.averaged.bitangent_size : m.face.bitangent_size);
}

auto Mesh::buildGlMesh() -> void {
	free();
	auto build_mesh = [](GlMesh& mesh) {
		mesh.mesh_size = static_cast<int>(mesh.vertices.size());
		glGenVertexArrays(1, &mesh.mesh_vao);
		glGenBuffers(1, &mesh.mesh_vbo);
		glBindVertexArray(mesh.mesh_vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.mesh_vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitangent)));
	};

	build_mesh(m.face);
	build_mesh(m.averaged);
	buildGlLines();
}

auto Mesh::buildGlLines() -> void {
	auto build_lines = [](GlMesh& mesh) {
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		float len = 0.3f;
		for (const auto& v : mesh.vertices) {
			normals.push_back(v.pos);
			normals.push_back(v.pos + v.normal * len);

			tangents.push_back(v.pos);
			tangents.push_back(v.pos + v.tangent * len);

			bitangents.push_back(v.pos);
			bitangents.push_back(v.pos + v.bitangent * len);
		}

		mesh.normal_size = static_cast<int>(normals.size());
		glGenVertexArrays(1, &mesh.normal_vao);
		glGenBuffers(1, &mesh.normal_vbo);
		glBindVertexArray(mesh.normal_vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.normal_vbo);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

		mesh.tangent_size = static_cast<int>(tangents.size());
		glGenVertexArrays(1, &mesh.tangent_vao);
		glGenBuffers(1, &mesh.tangent_vbo);
		glBindVertexArray(mesh.tangent_vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.tangent_vbo);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

		mesh.bitangent_size = static_cast<int>(bitangents.size());
		glGenVertexArrays(1, &mesh.bitangent_vao);
		glGenBuffers(1, &mesh.bitangent_vbo);
		glBindVertexArray(mesh.bitangent_vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.bitangent_vbo);
		glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), bitangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	};
	build_lines(m.face);
	build_lines(m.averaged);
}

auto Mesh::calculateTangents(std::vector<Vertex>& vertices) -> void {
	for (size_t i = 0; i < vertices.size(); i += 3) {
		Vertex& v0 = vertices[i];
		Vertex& v1 = vertices[i + 1];
		Vertex& v2 = vertices[i + 2];

		glm::vec3 edge1 = v1.pos - v0.pos;
		glm::vec3 edge2 = v2.pos - v0.pos;

		glm::vec2 delta_uv1 = v1.uv - v0.uv;
		glm::vec2 delta_uv2 = v2.uv - v0.uv;

		float denom = ((delta_uv1.x * delta_uv2.y) - (delta_uv2.x * delta_uv1.y));
		float f = (std::abs(denom) > 0.0001f) ? 1.0f / denom : 0.0f;

		glm::vec3 tangent;
		tangent.x = f * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x);
		tangent.y = f * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y);
		tangent.z = f * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z);

		glm::vec3 bitangent;
		bitangent.x = f * (-delta_uv2.x * edge1.x + delta_uv1.x * edge2.x);
		bitangent.y = f * (-delta_uv2.x * edge1.y + delta_uv1.x * edge2.y);
		bitangent.z = f * (-delta_uv2.x * edge1.z + delta_uv1.x * edge2.z);

		auto orthonormalize = [](Vertex& v, const glm::vec3& t, const glm::vec3& b) {
			// tangent
			if (glm::length(t) > 0.0001f) {
				v.tangent = glm::normalize(t - v.normal * glm::dot(v.normal, t));
			} else {
				v.tangent = t;
			}

			// bitangent
			if (glm::length(b) > 0.0001f) {
				v.bitangent = glm::normalize(b - (v.normal * glm::dot(v.normal, b)) - (v.tangent * glm::dot(v.tangent, b)));
			} else {
				v.bitangent = b;
			}
		};

		orthonormalize(v0, tangent, bitangent);
		orthonormalize(v1, tangent, bitangent);
		orthonormalize(v2, tangent, bitangent);
	}
}

auto Mesh::averageVertices(const std::vector<Vertex>& vertices) const -> std::vector<Vertex> {
	std::unordered_map<glm::ivec3, glm::vec3> key_to_normal_sum;
	std::unordered_map<glm::ivec3, glm::vec3> key_to_tangent_sum;
	std::unordered_map<glm::ivec3, glm::vec3> key_to_bitangent_sum;

	for (const auto& v : vertices) {
		glm::ivec3 key = glm::round(v.pos * 1e4f);
		key_to_normal_sum[key] += v.normal;
		key_to_tangent_sum[key] += v.tangent;
		key_to_bitangent_sum[key] += v.bitangent;
	}

	std::vector<Vertex> avg_verts = vertices;
	for (auto& v : avg_verts) {
		glm::ivec3 key = glm::round(v.pos * 1e4f);

		glm::vec3 avg_normal = key_to_normal_sum[key];
		if (glm::length(avg_normal) > 0.0001f) {
			v.normal = glm::normalize(avg_normal);
		} else {
			v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		}

		glm::vec3 avg_tangent = key_to_tangent_sum[key];
		glm::vec3 tangent = avg_tangent - v.normal * glm::dot(v.normal, avg_tangent);
		if (glm::length(tangent) > 0.0001f) {
			v.tangent = glm::normalize(tangent);
		} else {
			v.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
		}

		glm::vec3 avg_bitangent = key_to_bitangent_sum[key];
		glm::vec3 bitangent = glm::cross(v.normal, v.tangent);
		if (glm::dot(bitangent, avg_bitangent) < 0.0f) {
			bitangent = -bitangent;
		}
		v.bitangent = glm::normalize(bitangent);
	}
	return avg_verts;
}

void Mesh::loadObj(const std::string& filename) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str())) {
		std::cerr << "TinyObj error: " << err << '\n';
		return;
	}

	std::vector<Vertex> vertices;
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex v {};
			v.pos = {
			  attrib.vertices[(3 * index.vertex_index) + 0],
			  attrib.vertices[(3 * index.vertex_index) + 1],
			  attrib.vertices[(3 * index.vertex_index) + 2]
			};
			if (index.normal_index >= 0) {
				v.normal = {
				  attrib.normals[(3 * index.normal_index) + 0],
				  attrib.normals[(3 * index.normal_index) + 1],
				  attrib.normals[(3 * index.normal_index) + 2]
				};
			}
			if (index.texcoord_index >= 0) {
				v.uv = {attrib.texcoords[(2 * index.texcoord_index) + 0], attrib.texcoords[(2 * index.texcoord_index) + 1]};
			}
			vertices.push_back(v);
		}
	}
	calculateTangents(vertices);
	m.face.vertices = vertices;
	m.averaged.vertices = vertices;
	buildGlMesh();
}

/////////////////////////////////////////////////////////// GENERATED MESHES //////////////////////////////////////////

void Mesh::generatePlane() {
	std::vector<Vertex> vertices = {
	  {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	  { {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	  {  {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	  {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	  {  {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	  { {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
	};
	calculateTangents(vertices);
	m.face.vertices = vertices;
	m.averaged.vertices = vertices;
	buildGlMesh();
}

void Mesh::generateCube() {
	std::vector<Vertex> vertices;
	auto add_face = [&](glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 n) {
		vertices.push_back({
		  p1, n, {0, 0}
		});
		vertices.push_back({
		  p2, n, {1, 0}
		});
		vertices.push_back({
		  p3, n, {1, 1}
		});
		vertices.push_back({
		  p1, n, {0, 0}
		});
		vertices.push_back({
		  p3, n, {1, 1}
		});
		vertices.push_back({
		  p4, n, {0, 1}
		});
	};

	add_face({-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}, {0, 0, 1});
	add_face({0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}, {0, 0, -1});
	add_face({-0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}, {0, 1, 0});
	add_face({-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}, {-0.5, -0.5, 0.5}, {0, -1, 0});
	add_face({-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5}, {-0.5, 0.5, 0.5}, {-0.5, 0.5, -0.5}, {-1, 0, 0});
	add_face({0.5, -0.5, 0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}, {1, 0, 0});

	calculateTangents(vertices);
	m.face.vertices = vertices;
	m.averaged.vertices = averageVertices(vertices);
	buildGlMesh();
}

void Mesh::generateCone(int slices) {
	std::vector<Vertex> vertices;
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
		vertices.push_back({
		  center, down, {umid, 0.0f}
		});
		vertices.push_back({
		  p1, down, {u1, 0.0f}
		});
		vertices.push_back({
		  p2, down, {u2, 0.0f}
		});

		glm::vec3 fn = glm::normalize(glm::cross(p1 - p2, tip - p2));
		vertices.push_back({
		  p2, fn, {u2, 0.0f}
		});
		vertices.push_back({
		  p1, fn, {u1, 0.0f}
		});
		vertices.push_back({
		  tip, fn, {umid, 1.0f}
		});
	}

	calculateTangents(vertices);
	m.face.vertices = vertices;
	m.averaged.vertices = averageVertices(vertices);
	buildGlMesh();
}

void Mesh::generateCylinder(int slices) {
	std::vector<Vertex> vertices;
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
		vertices.push_back({
		  top_center, up, {umid, 1.0f}
		});
		vertices.push_back({
		  t2, up, {u2, 1.0f}
		});
		vertices.push_back({
		  t1, up, {u1, 1.0f}
		});

		glm::vec3 down = {0, -1, 0};
		vertices.push_back({
		  bot_center, down, {umid, 0.0f}
		});
		vertices.push_back({
		  b1, down, {u1, 0.0f}
		});
		vertices.push_back({
		  b2, down, {u2, 0.0f}
		});

		glm::vec3 fn1 = glm::normalize(glm::cross(b1 - b2, t1 - b2));
		glm::vec3 fn2 = glm::normalize(glm::cross(t1 - b2, t2 - b2));

		vertices.push_back({
		  b2, fn1, {u2, 0.0f}
		});
		vertices.push_back({
		  b1, fn1, {u1, 0.0f}
		});
		vertices.push_back({
		  t1, fn1, {u1, 1.0f}
		});

		vertices.push_back({
		  b2, fn2, {u2, 0.0f}
		});
		vertices.push_back({
		  t1, fn2, {u1, 1.0f}
		});
		vertices.push_back({
		  t2, fn2, {u2, 1.0f}
		});
	}

	calculateTangents(vertices);
	m.face.vertices = vertices;
	m.averaged.vertices = averageVertices(vertices);
	buildGlMesh();
}

void Mesh::generateSphere(int slices, int rings) {
	std::vector<Vertex> vertices;
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
				vertices.push_back({a, fn, uva});
				vertices.push_back({b, fn, uvb});
				vertices.push_back({c, fn, uvc});
			};

			if (i != rings - 1) {
				add_tri(p4, p3, p1, uv4, uv3, uv1);
			}
			if (i != 0) {
				add_tri(p4, p1, p2, uv4, uv1, uv2);
			}
		}
	}

	calculateTangents(vertices);
	m.face.vertices = vertices;
	m.averaged.vertices = averageVertices(vertices);
	buildGlMesh();
}
