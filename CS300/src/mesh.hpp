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
		GLuint vbo = 0;
		GLuint vao = 0;
	} m;

public:
	Mesh(std::vector<myVertex> verts) {
		m.vertices = std::move(verts);

		glGenVertexArrays(1, &m.vao);
		glGenBuffers(1, &m.vbo);

		glBindVertexArray(m.vao);
		glBindBuffer(GL_ARRAY_BUFFER, m.vbo);

		glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(myVertex), m.vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(myVertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(myVertex), (void*)offsetof(myVertex, color));

		glBindVertexArray(0);
	}

	~Mesh() {
		if (m.vao) {
			glDeleteVertexArrays(1, &m.vao);
		}
		if (m.vbo) {
			glDeleteBuffers(1, &m.vbo);
		}
	}

	Mesh(const Mesh&) = delete;
	auto operator=(const Mesh&) -> Mesh& = delete;

	Mesh(Mesh&& other) noexcept : m(std::move(other.m)) {
		other.m.vao = 0;
		other.m.vbo = 0;
	}

	static auto createTri() -> std::unique_ptr<Mesh> {
		std::vector<myVertex> verts = {
		  {  {0.75f, 0.75f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		  { {0.75f, -0.75f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		  {{-0.75f, -0.75f, 0.0f}, {0.0f, 0.0f, 1.0f}}
		};
		return std::make_unique<Mesh>(std::move(verts));
	}

	void draw() const {
		if (m.vao == 0) {
			return;
		}
		glBindVertexArray(m.vao);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m.vertices.size()));
		glBindVertexArray(0);
	}
};
