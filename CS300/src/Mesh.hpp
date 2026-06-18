#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent = {0, 0, 0};
	glm::vec3 bitangent = {0, 0, 0};
};

class Mesh {
public:
	enum class Type : char {
		plane,
		cube,
		cone,
		cylinder,
		sphere,
		other
	};

	Mesh(Type type, int slices = 4, int rings = 2);
	Mesh(const std::string& filename);
	~Mesh();

	Mesh(const Mesh&) = delete;
	auto operator=(const Mesh&) -> Mesh& = delete;
	Mesh(Mesh&& other) noexcept = delete;

	void draw(bool average) const;
	void drawNormals(bool average) const;
	void drawTangents(bool average) const;
	void drawBitangents(bool average) const;

private:
	struct GlMesh {
		std::vector<Vertex> vertices;
		// Main Mesh Data
		GLuint mesh_vao = 0;
		GLuint mesh_vbo = 0;
		int mesh_size = 0;

		// Normal Line Mesh Data
		GLuint normal_vao = 0;
		GLuint normal_vbo = 0;
		int normal_size = 0;

		// tangent Line Mesh Data
		GLuint tangent_vao = 0;
		GLuint tangent_vbo = 0;
		int tangent_size = 0;

		// bitangent Line Mesh Data
		GLuint bitangent_vao = 0;
		GLuint bitangent_vbo = 0;
		int bitangent_size = 0;
	};

	struct {
		GlMesh face;
		GlMesh averaged;
	} m;

	auto buildGlMesh() -> void;
	auto buildGlLines() -> void;
	auto calculateTangents(std::vector<Vertex>& vertices) -> void;
	auto averageVertices(const std::vector<Vertex>& vertices) const -> std::vector<Vertex>;

	auto free() -> void;

	// before running these function clean up open gl stuff
	void loadObj(const std::string& filename);
	void generatePlane();
	void generateCube();
	void generateCone(int slices);
	void generateCylinder(int slices);
	void generateSphere(int slices, int rings);
};
