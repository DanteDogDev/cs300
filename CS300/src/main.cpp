#include "Camera.hpp"
#include "Light.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "cs300/CS300Parser.h"
#if _DEBUG
#include "cs300/OGLDebug.h"
#endif

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
	bool draw_lines = false;
	bool draw_textures = true;
	bool average_lines = true;
	bool wireframe = false;
	int slices = 4;
	int rings = 2;
	bool quit = false;
	int render_mode = 0;
} STATE;

auto main(int argc, char* args[]) -> int {
	// SETUP SDL
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return 1;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_Window* window = SDL_CreateWindow("CS300 Assignment 2", 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	glewInit();

#if _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, nullptr);
#endif

	std::map<std::string, Shader> programs;
	programs.insert({"phong", Shader("data/shaders/phong.vert", "data/shaders/phong.frag")});
	programs.insert({"normal", Shader("data/shaders/normal.vert", "data/shaders/normal.frag")});
	programs.insert({"depth", Shader("data/shaders/depth.vert", "data/shaders/depth.frag")});
	programs.insert({"texture", Shader("data/shaders/texture.vert", "data/shaders/texture.frag")});

	Texture texture;

	CS300Parser parser;
	parser.LoadDataFromFile("data/scenes/scene_A4.txt");

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

	std::map<std::string, std::shared_ptr<Texture>> texture_cache;
	texture_cache.emplace("default", std::make_shared<Texture>());
	auto add_texture = [&texture_cache](std::string file_path) {
		texture_cache.emplace(file_path, std::make_shared<Texture>(file_path));
	};
	add_texture("data/textures/default_normal.png");
	add_texture("data/textures/brick_normal_map.png");
	add_texture("data/textures/circles_normal_map.png");
	auto get_texture = [&texture_cache](const std::string& file_path) {
		if (texture_cache.find(file_path) == texture_cache.end()) {
			return std::shared_ptr<Texture>(nullptr);
		}
		return texture_cache[file_path];
	};

	auto build_objects = [&]() {
		std::vector<Object> objects;
		size_t count = parser.objects.size();
		SDL_Log("CRITICAL: parser.objects.size() returned an invalid value: %zu", count);
		objects.reserve(count);
		for (const auto& o : parser.objects) {
			objects.emplace_back(get_mesh(o.mesh), o.pos, o.rot, o.sca, o.ns, o.anims);
		}
		return objects;
	};
	std::vector<Object> objects = build_objects();

	// std::vector<std::unique_ptr<Light>> lights;
	// for (const auto& l : parser.lights) {
	// 	if (static_cast<int>(lights.size()) >= LIGHT_NUM_MAX) {
	// 		break;
	// 	}
	// 	lights.push_back(Light::create(l));
	// }

	// GLuint depth_fbo;
	// glGenFramebuffers(1, &depth_fbo);
	// glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	//
	// auto light_tex = Texture(512, 512, GL_DEPTH_COMPONENT);
	// light_tex.setDepthBuffer();
	//
	// glDrawBuffer(GL_NONE);
	// glReadBuffer(GL_NONE);
	//
	// if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	// 	SDL_Log("ERROR::FRAMEBUFFER:: Depth framebuffer is not complete!");
	// }
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//

	// GLuint cubemap;
	// GLuint cubeFBO[6];
	//
	// glGenTextures(1, &cubemap);
	// glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	//
	// const unsigned EnvMapSize = 512;
	// for (GLuint i = 0; i < 6; i++) {
	// 	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, EnvMapSize, EnvMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	// }
	//
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//
	// // Create and set up the FBO
	// glGenFramebuffers(6, cubeFBO);
	// for (GLuint i = 0; i < 6; i++) {
	// 	glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO[i]);
	// 	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
	// 	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
	// 	glDrawBuffers(1, drawBuffers);
	// }
	//
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
					case SDL_SCANCODE_N: STATE.draw_lines = !STATE.draw_lines; break;
					case SDL_SCANCODE_T: STATE.render_mode = (STATE.render_mode + 1) % 4; break;
					case SDL_SCANCODE_F: STATE.average_lines = !STATE.average_lines; break;
					case SDL_SCANCODE_M: STATE.wireframe = !STATE.wireframe; break;
					case SDL_SCANCODE_EQUALS:
					case SDL_SCANCODE_Z:
						STATE.slices++;
						STATE.rings = std::max(2, STATE.slices / 2);
						mesh_cache.clear();
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
		// for (auto& light : lights) {
		// 	light->update(time);
		// }

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

		// PASS ONE TODO:

		// glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
		// glViewport(0, 0, 512, 512);
		// glClear(GL_DEPTH_BUFFER_BIT);
		//
		// auto& main_light = lights.at(0);
		// auto light_cam = main_light->getCamera();
		//
		// auto t = glm::mat4(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
		// auto matrix = light_cam.getProj() * light_cam.getView();
		// auto light_space_matrix = t * light_cam.getProj() * light_cam.getView();
		//
		// const static Shader& depth = programs.at("depth");
		// depth.use();
		// for (const auto& obj : objects) {
		// 	if (obj.m.mesh == get_mesh("PLANE")) {
		// 		continue;
		// 	}
		// 	depth.setUniform("uMVP", matrix * obj.getModelMatrix());
		// 	obj.m.mesh->draw(false);
		// }

		// PASS TWO TODO:

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1280, 720);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const static Shader& phong = programs.at("phong");
		phong.use();
		phong.setUniform("uDiffuseTex", 0);
		texture.bind(0);
		// phong.setUniform("uNormalTex", 1);
		// phong.setUniform("uLightTex", 2);
		// light_tex.bind(2);

		// phong.setUniform("uLightSpaceMat", light_space_matrix);
		// phong.setUniform("uView", camera.getView());
		// phong.setUniform("uRenderMode", STATE.render_mode);
		// phong.setUniform("uLightNum", static_cast<int>(lights.size()));
		// for (size_t i = 0; i < lights.size(); ++i) {
		// 	lights[i]->setUniforms(phong, static_cast<int>(i));
		// }

		for (const auto& obj : objects) {
			obj.draw(phong, view_proj, STATE.average_lines, STATE.draw_textures);
		}

		const static Shader& normal_shader = programs.at("normal");
		normal_shader.use();

		// glUniform4f(1, 1.0f, 1.0f, 1.0f, 1.0f);
		// for (const auto& obj : lights) {
		// 	auto sphere = get_mesh("SPHERE");
		// 	glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), obj->m.curr_pos), glm::vec3(3, 3, 3));
		// 	normal_shader.setUniform("uMVP", view_proj * model);
		//
		// 	sphere->draw(false);
		// }

		if (STATE.draw_lines) {
			for (const auto& obj : objects) {
				obj.drawLines(normal_shader, view_proj, STATE.average_lines);
			}
		}
		// if (STATE.render_mode % 2 == 0) {
		// 	// render thingy
		// 	glViewport(-180, -180, 720, 720);
		// 	glDisable(GL_DEPTH_TEST);
		// 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// 	const static Shader& texture = programs.at("texture");
		// 	texture.use();
		// 	texture.setUniform("uTex", 2);
		// 	get_mesh("PLANE")->draw(false);
		// 	glEnable(GL_DEPTH_TEST);
		// }

		SDL_GL_SwapWindow(window);
	}

	// glDeleteFramebuffers(1,&depth_fbo);

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
