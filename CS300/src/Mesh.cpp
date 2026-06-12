#define TINYOBJLOADER_IMPLEMENTATION
#include "Mesh.hpp"

#include "cs300/tiny_obj_loader.h"

#include <cmath>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <map>
#include <array> // For std::array

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
	std::array<GLuint, 4> vaos = {m.vao_face, m.vao_avg, m.vao_norm_lines_face, m.vao_norm_lines_avg};
	std::array<GLuint, 4> vbos = {m.vbo_face, m.vbo_avg, m.vbo_norm_lines_face, m.vbo_norm_lines_avg};
	glDeleteVertexArrays(4, vaos.data());
	glDeleteBuffers(4, vbos.data());
}

Mesh::Mesh(Mesh&& other) noexcept {
	m = other.m;
	other.m = {};
}

auto Mesh::operator=(Mesh&& other) noexcept -> Mesh& {
	if (this != &other) {
		this->~Mesh();
		m = other.m;
		other.m = {};
	}
	return *this;
}

void Mesh::draw(bool averaged_normals) const {
	glBindVertexArray(averaged_normals ? m.vao_avg : m.vao_face);
	glDrawArrays(GL_TRIANGLES, 0, averaged_normals ? m.vertex_count_avg : m.vertex_count_face);
}

void Mesh::drawNormals(bool averaged_normals) const {
	glBindVertexArray(averaged_normals ? m.vao_norm_lines_avg : m.vao_norm_lines_face);
	glDrawArrays(GL_LINES, 0, averaged_normals ? m.norm_line_count_avg : m.norm_line_count_face);
}

void Mesh::setupGl(const std::vector<Vertex>& face_verts, const std::vector<Vertex>& avg_verts) {
	m.vertex_count_face = static_cast<int>(face_verts.size());
	m.vertex_count_avg = static_cast<int>(avg_verts.size());

	auto setup = [](GLuint& vao, GLuint& vbo, const std::vector<Vertex>& verts) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
	};

	setup(m.vao_face, m.vbo_face, face_verts);
	setup(m.vao_avg, m.vbo_avg, avg_verts);
	setupNormalLines(face_verts, avg_verts);
}

void Mesh::setupNormalLines(const std::vector<Vertex>& face_verts, const std::vector<Vertex>& avg_verts) {
	auto create_lines = [](const std::vector<Vertex>& verts) {
		std::vector<glm::vec3> lines;
		float len = 0.5f;
		for (const auto& v : verts) {
			lines.push_back(v.pos);
			lines.push_back(v.pos + v.normal * len);
		}
		return lines;
	};

	auto face_lines = create_lines(face_verts);
	auto avg_lines = create_lines(avg_verts);
	m.norm_line_count_face = static_cast<int>(face_lines.size());
	m.norm_line_count_avg = static_cast<int>(avg_lines.size());

	auto setup = [](GLuint& vao, GLuint& vbo, const std::vector<glm::vec3>& lines) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(glm::vec3), lines.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	};

	setup(m.vao_norm_lines_face, m.vbo_norm_lines_face, face_lines);
	setup(m.vao_norm_lines_avg, m.vbo_norm_lines_avg, avg_lines);
}

static auto quantizePos(const glm::vec3& p) -> glm::ivec3 {
	const float grid = 1e4f;
	return glm::round(p * grid);
}

struct IVec3Less {    // TODO: replace this

	auto operator()(const glm::ivec3& a, const glm::ivec3& b) const -> bool {
		if (a.x != b.x) {
			return a.x < b.x;
		}
		if (a.y != b.y) {
			return a.y < b.y;
		}
		return a.z < b.z;
	}
};

auto Mesh::computeAveragedNormals(const std::vector<Vertex>& face_verts) const -> std::vector<Vertex> {
	std::map<glm::ivec3, glm::vec3, IVec3Less> key_to_sum;
	for (const auto& v : face_verts) {
		key_to_sum[quantizePos(v.pos)] += v.normal;
	}

	std::vector<Vertex> avg_verts = face_verts;
	for (auto& v : avg_verts) {
		v.normal = glm::normalize(key_to_sum[quantizePos(v.pos)]);
	}
	return avg_verts;
}

void Mesh::generatePlane() {
	std::vector<Vertex> verts = {
	  {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	  { {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	  {  {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	  {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	  {  {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	  { {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
	};
	setupGl(verts, verts);
}

void Mesh::generateCube() {
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

	setupGl(face_verts, computeAveragedNormals(face_verts));
}

void Mesh::generateCone(int slices) {
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

	setupGl(face_verts, computeAveragedNormals(face_verts));
}

void Mesh::generateCylinder(int slices) {
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

	setupGl(face_verts, computeAveragedNormals(face_verts));
}

void Mesh::generateSphere(int slices, int rings) {
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
	setupGl(face_verts, computeAveragedNormals(face_verts));
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

	std::vector<Vertex> face_verts;
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
			face_verts.push_back(v);
		}
	}
	setupGl(face_verts, face_verts);
}
