#pragma once

#include "camera.hpp"
#include "cs300/CS300Parser.h"
#include "generate.hpp"
#include "object.hpp"
#include "open_gl.hpp"
#include "window.hpp"

#include <SDL3/SDL_events.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class World {
public:
	World(GLsizei width, GLsizei height);
	~World();

	void init();
	void handleKeyInput(SDL_Scancode scancode);
	void render(Window& window);
	void update(float current_time_seconds);
	void clear();

private:
	GLsizei m_width;
	GLsizei m_height;

	bool render_texture;
	bool render_normals;
	bool render_normals_averaged;

	std::unique_ptr<Camera> camera;
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Object>> lights;

	gl::Texture* default_texture;
	gl::Shader* shader;
	cs300::Mesh* default_mesh;
	int slices;
};
