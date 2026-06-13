#include "Camera.hpp"
#include "Light.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "cs300/CS300Parser.h"
#include "cs300/OGLDebug.h"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <vector>

struct AppState {
	float r = 110.0f;
	float alpha = 90.0f;
	float beta = 0.0f;
	bool draw_normals = false;
	bool draw_textures = true;
	bool averaged_normals = false;
	bool wireframe = false;
	int slices = 4;
	int rings = 2;
	bool quit = false;
} STATE;

auto main(int argc, char* args[]) -> int {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("CS300 Assignment 1", 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	glewInit();

#if _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif

	std::map<std::string, Shader> programs;
	programs.insert({"phong", Shader("data/shaders/phong.vert", "data/shaders/phong.frag")});
	programs.insert({"normal", Shader("data/shaders/normal.vert", "data/shaders/normal.frag")});

	Texture texture;

	CS300Parser parser;
	parser.LoadDataFromFile("data/scenes/scene_A1.txt");

	Camera camera(parser.fovy, 1280.0f / 720.0f, parser.nearPlane, parser.farPlane);

	std::map<std::string, std::shared_ptr<Mesh>> mesh_cache;
	auto get_mesh = [&](const std::string& name) -> std::shared_ptr<Mesh> {
		if (mesh_cache.count(name)) {
			return mesh_cache[name];
		}

		if (name == "PLANE") {
			return mesh_cache[name] = std::make_shared<Mesh>(Mesh::Type::plane);
		}
		if (name == "CUBE") {
			return mesh_cache[name] = std::make_shared<Mesh>(Mesh::Type::cube);
		}
		if (name == "CONE") {
			return mesh_cache[name] = std::make_shared<Mesh>(Mesh::Type::cone, STATE.slices);
		}
		if (name == "CYLINDER") {
			return mesh_cache[name] = std::make_shared<Mesh>(Mesh::Type::cylinder, STATE.slices);
		}
		if (name == "SPHERE") {
			return mesh_cache[name] = std::make_shared<Mesh>(Mesh::Type::sphere, STATE.slices, STATE.rings);
		}

		return mesh_cache[name] = std::make_shared<Mesh>(name);
	};

	auto build_objects = [&]() {
		std::vector<Object> objects;
		objects.reserve(parser.objects.size());
		for (const auto& o : parser.objects) {
			objects.emplace_back(get_mesh(o.mesh), o.pos, o.rot, o.sca, o.ns, o.anims);
		}
		return objects;
	};
	std::vector<Object> objects = build_objects();

	std::vector<std::unique_ptr<Light>> lights;
	for (const auto& l : parser.lights) {
		if (static_cast<int>(lights.size()) >= LIGHT_NUM_MAX) {
			break;
		}
		lights.push_back(Light::create(l));
	}

	SDL_Event event;
	int numkeys;
	const bool* keys = SDL_GetKeyboardState(&numkeys);

	while (!STATE.quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				STATE.quit = true;
			}
			if (event.type == SDL_EVENT_KEY_DOWN) {
				switch (event.key.scancode) {
					case SDL_SCANCODE_ESCAPE: STATE.quit = true; break;
					case SDL_SCANCODE_N: STATE.draw_normals = !STATE.draw_normals; break;
					case SDL_SCANCODE_T: STATE.draw_textures = !STATE.draw_textures; break;
					case SDL_SCANCODE_F: STATE.averaged_normals = !STATE.averaged_normals; break;
					case SDL_SCANCODE_M: STATE.wireframe = !STATE.wireframe; break;
					case SDL_SCANCODE_EQUALS:    // +
					case SDL_SCANCODE_Z:
						STATE.slices++;
						STATE.rings = std::max(2, STATE.slices / 2);
						mesh_cache.clear();    // Rebuild procedural meshes
						objects = build_objects();
						break;
					case SDL_SCANCODE_MINUS:
					case SDL_SCANCODE_X:
						if (STATE.slices > 4) {
							STATE.slices--;
							STATE.rings = std::max(2, STATE.slices / 2);
							mesh_cache.clear();
							objects = build_objects();
						}
						break;
					default: break;
				}
			}
		}

		if (keys[SDL_SCANCODE_W]) {
			STATE.alpha -= 0.5f;
		}
		if (keys[SDL_SCANCODE_S]) {
			STATE.alpha += 0.5f;
		}
		if (keys[SDL_SCANCODE_A]) {
			STATE.beta -= 0.5f;
		}
		if (keys[SDL_SCANCODE_D]) {
			STATE.beta += 0.5f;
		}
		if (keys[SDL_SCANCODE_Q]) {
			STATE.r -= 0.1f;
		}
		if (keys[SDL_SCANCODE_E]) {
			STATE.r += 0.1f;
		}

		STATE.alpha = std::max(STATE.alpha, 1.0f);
		STATE.alpha = std::min(STATE.alpha, 179.0f);
		STATE.r = std::max(STATE.r, 0.1f);

		float time = SDL_GetTicks() / 1000.0f;
		for (auto& obj : objects) {
			obj.update(time);
		}
		for (auto& light : lights) {
			light->update(time);
		}

		camera.update(STATE.r, STATE.alpha, STATE.beta, parser.camTarget);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		if (STATE.wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glm::mat4 view_proj = camera.getProj() * camera.getView();

		const Shader& shader = programs.at("phong");
		shader.use();
		texture.bind(0);
		shader.setUniform("uDiffuseTex", 0);
		shader.setUniform("uCamPos", camera.getPosition());
		shader.setUniform("uLightNum", static_cast<int>(lights.size()));
		for (size_t i = 0; i < lights.size(); ++i) {
			lights[i]->setUniforms(shader, static_cast<int>(i));
		}

		for (const auto& obj : objects) {
			obj.draw(shader, view_proj, STATE.averaged_normals, STATE.draw_textures);
		}

		if (STATE.draw_normals) {
			const Shader& normal_shader = programs.at("normal");
			normal_shader.use();
			for (const auto& obj : objects) {
				obj.drawNormals(normal_shader, view_proj, STATE.averaged_normals);
			}
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
