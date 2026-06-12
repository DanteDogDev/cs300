#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
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
	Mesh(Mesh&& other) noexcept;
	auto operator=(Mesh&& other) noexcept -> Mesh&;

	void draw(bool averaged_normals) const;
	void drawNormals(bool averaged_normals) const;

private:
	struct {
		GLuint vao_face = 0;
		GLuint vbo_face = 0;
		GLuint vao_avg = 0;
		GLuint vbo_avg = 0;

		GLuint vao_norm_lines_face = 0;
		GLuint vbo_norm_lines_face = 0;
		GLuint vao_norm_lines_avg = 0;
		GLuint vbo_norm_lines_avg = 0;

		int vertex_count_face = 0;
		int vertex_count_avg = 0;
		int norm_line_count_face = 0;
		int norm_line_count_avg = 0;
	} m;

	void generatePlane();
	void generateCube();
	void generateCone(int slices);
	void generateCylinder(int slices);
	void generateSphere(int slices, int rings);
	void loadObj(const std::string& filename);

	void setupGl(const std::vector<Vertex>& face_verts, const std::vector<Vertex>& avg_verts);
	void setupNormalLines(const std::vector<Vertex>& face_verts, const std::vector<Vertex>& avg_verts);
	auto computeAveragedNormals(const std::vector<Vertex>& face_verts) -> std::vector<Vertex>;
};
