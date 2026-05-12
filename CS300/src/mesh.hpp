/**
 * @file mesh.hpp
 * @author Dante Harper
 * @date 12 May 26
 */

#pragma once

#include "OGLDebug.h"

#include <cstdio>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

struct myVertex {
	glm::vec3 pos;
	glm::vec3 color;
};

class Mesh {
	struct {
		std::vector<myVertex> vertices;

		GLuint vbo;    // vertices
		GLuint vao;    // vertex array object
	} m;

public:

	Mesh(std::vector<myVertex> verts) {
		std::printf("Creating Mesh\n");
		m.vertices = std::move(verts);
		// VAO
		glGenVertexArrays(1, &m.vao);

		// VBO
		glGenBuffers(1, &m.vbo);

		glBindVertexArray(m.vao);
		glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(myVertex), verts.data(), GL_STATIC_DRAW);

		// Insert the VBO into the VAO
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(myVertex), nullptr);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(myVertex), reinterpret_cast<void*>(offsetof(myVertex, color)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	~Mesh() {
		std::printf("Destroying Mesh\n");
		glDeleteBuffers(1, &m.vbo);
		glDeleteVertexArrays(1, &m.vao);
	}

	static auto createTri() -> Mesh {
		std::vector<myVertex> verts = {
		  {  {0.75f, 0.75f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Red top-right
		  { {0.75f, -0.75f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Green bottom-right
		  {{-0.75f, -0.75f, 0.0f}, {0.0f, 0.0f, 1.0f}}  // Blue bottom-left
		};
		return {verts};
	}

	void draw() const {
		glBindVertexArray(m.vao);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m.vertices.size()));
		glBindVertexArray(0);
	}
};
