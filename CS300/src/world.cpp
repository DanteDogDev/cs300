#include "world.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

World::World(GLsizei width, GLsizei height)
    : m_width(width),
      m_height(height),
      render_texture(false),
      render_normals(false),
      render_normals_averaged(false),
      slices(4) {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

World::~World() {
	clear();
}

void World::init() {
	CS300Parser parser;
	Manager::init(slices, slices / 2);

	parser.LoadDataFromFile("./data/scenes/scene_A1.txt");

	if (!parser.objects.empty()) {
		for (const auto& transform_data : parser.objects) {
			objects.push_back(std::make_unique<Object>(transform_data));
		}
	}

	if (!parser.lights.empty()) {
		for (const auto& light_data : parser.lights) {
			lights.push_back(std::make_unique<Object>(light_data));
		}
	}

	camera = std::make_unique<Camera>(parser);
	default_texture = Manager::getTexture("./data/textures/default.bmp");
	shader = Manager::getShader("default");
	default_mesh = Manager::getMesh("SPHERE");
}

void World::render(Window& window) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->updatePosition();
	glm::mat4 view_matrix = camera->getViewMatrix();
	glm::mat4 projection_matrix = camera->getProjectionMatrix();

	shader->bind();
	shader->setUniform("view", view_matrix);
	shader->setUniform("projection", projection_matrix);

	shader->setUniform("uLightNum", int(lights.size()));
	for (size_t i = 0; i < lights.size(); i++) {
		// shader->setUniform("uLight[" + std::to_string(i) + "].type", 0);
		// shader->setUniform("uLight[" + std::to_string(i) + "].ambient", lights[i]->light.col);
		// shader->setUniform("uLight[" + std::to_string(i) + "].diffuse", lights[i]->light.col);
		// shader->setUniform("uLight[" + std::to_string(i) + "].specular", lights[i]->light.col);
		// shader->setUniform("uLight[" + std::to_string(i) + "].positionWorld", lights[i]->transform.pos);
	}

	for (const auto& obj : objects) {
		auto* mesh = obj->mesh;
		shader->setUniform("model", obj->model_matrix);

		shader->setUniform("drawTex", render_texture);
		if (render_texture) {
			default_texture->bind(0);
			shader->setUniform("tex", 0);
		}

		mesh->draw();
		if (render_normals) {
			if (render_normals_averaged) {
				mesh->drawAveragedNormals();
			} else {
				mesh->drawNormals();
			}
		}
	}
	for (const auto& obj : lights) {
		auto* mesh = default_mesh;
		shader->setUniform("model", obj->model_matrix);
		shader->setUniform("drawTex", render_texture);

		mesh->draw();
	}
	shader->unbind();
}

void World::update(float current_time_seconds) {
	for (const auto& obj : objects) {
		obj->update(current_time_seconds);
	}
	for (const auto& obj : lights) {
		obj->update(current_time_seconds);
	}
}

void World::clear() {
	Manager::clear();
}

void World::handleKeyInput(SDL_Scancode scancode) {
	const bool* keys = SDL_GetKeyboardState(nullptr);

	if (keys[SDL_SCANCODE_W]) {
		camera->alpha -= 0.01f;
	}
	if (keys[SDL_SCANCODE_S]) {
		camera->alpha += 0.01f;
	}
	if (keys[SDL_SCANCODE_A]) {
		camera->beta -= 0.01f;
	}
	if (keys[SDL_SCANCODE_D]) {
		camera->beta += 0.01f;
	}
	if (keys[SDL_SCANCODE_E]) {
		camera->radius += 0.1f;
	}
	if (keys[SDL_SCANCODE_Q]) {
		camera->radius -= 0.1f;
		camera->radius = std::max(camera->radius, 0.1f);
	}

	if (scancode == SDL_SCANCODE_N) {
		render_normals = !render_normals;
	}
	if (scancode == SDL_SCANCODE_T) {
		render_texture = !render_texture;
	}
	if (scancode == SDL_SCANCODE_F) {
		render_normals_averaged = !render_normals_averaged;
	}
	if (scancode == SDL_SCANCODE_M) {
		static bool is_wireframe = false;
		is_wireframe = !is_wireframe;

		if (is_wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
		} else {
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (scancode == SDL_SCANCODE_KP_PLUS || scancode == SDL_SCANCODE_EQUALS || scancode == SDL_SCANCODE_Z) {
		slices++;
		Manager::regenerate(slices, slices / 2);
	}
	if (scancode == SDL_SCANCODE_KP_MINUS || scancode == SDL_SCANCODE_MINUS || scancode == SDL_SCANCODE_X) {
		slices--;
		slices = std::max(4, slices);
		Manager::regenerate(slices, slices / 2);
	}
}
