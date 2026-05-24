#include "SDL3/SDL_events.h"
#include "camera.hpp"
#include "cs300/CS300Parser.h"
#include "cs300/OGLDebug.h"
#include "generate.hpp"
#include "object.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "resources.hpp"
#include "tiny_obj_loader.h"
#undef TINYOBJLOADER_IMPLEMENTATION

#include <SDL3/SDL.h>
#include <iostream>
#include <memory>
// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

static int win_id;
static GLsizei width = 1280;
static GLsizei height = 720;

static bool render_normals = false;
static bool render_normals_averaged = false;
static std::vector<std::unique_ptr<Object>> objects;
static CS300Parser parser;
static std::unique_ptr<Camera> camera;

static int slices = 4;

void init() {
	ResourceManager::instance().init();
	generated::init(slices, slices / 2);

	parser.LoadDataFromFile("./data/scenes/scene_A0.txt");

	if (!parser.objects.empty()) {
		for (const auto& transform_data : parser.objects) {
			objects.push_back(std::make_unique<Object>(transform_data, transform_data.mesh));
		}
	}

	camera = std::make_unique<Camera>(parser);
}

void handleKeyInput(SDL_Scancode scancode) {
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
		// Toggle texture-mapping on/off
	}
	if (scancode == SDL_SCANCODE_F) {
		render_normals_averaged = !render_normals_averaged;
	}
	if (scancode == SDL_SCANCODE_M) {
		static bool is_wireframe = false;
		is_wireframe = !is_wireframe;

		if (is_wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (scancode == SDL_SCANCODE_KP_PLUS || scancode == SDL_SCANCODE_EQUALS || scancode == SDL_SCANCODE_Z) {
		slices++;
		generated::regenerate(slices, slices / 2);
	}
	if (scancode == SDL_SCANCODE_KP_MINUS || scancode == SDL_SCANCODE_MINUS || scancode == SDL_SCANCODE_X) {
		slices--;
		slices = std::max(4, slices);
		generated::regenerate(slices, slices / 2);
	}
}

void display(SDL_Window* window) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->updatePosition();
	glm::mat4 view_matrix = camera->getViewMatrix();
	glm::mat4 projection_matrix = camera->getProjectionMatrix();

	for (const auto& object_ptr : objects) {
		object_ptr->shader->bind();
		object_ptr->shader->setUniformMat4("model", object_ptr->model_matrix);
		object_ptr->shader->setUniformMat4("view", view_matrix);
		object_ptr->shader->setUniformMat4("projection", projection_matrix);

		// object_ptr->texture->Bind(0);
		// object_ptr->shader->setUniform1i("tex", 0);

		object_ptr->mesh->draw();
		if (render_normals) {
			if (render_normals_averaged) {
				object_ptr->mesh->drawAveragedNormals();
			} else {
				object_ptr->mesh->drawNormals();
			}
		}

		object_ptr->shader->unbind();
	}
	SDL_GL_SwapWindow(window);
}

auto main(int argc, char* args[]) -> int {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "Could not initialize SDL: " << SDL_GetError() << '\n';
		exit(1);
	}

	SDL_Window* window = SDL_CreateWindow("CS300", width, height, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
		SDL_Quit();
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == nullptr) {
		SDL_DestroyWindow(window);
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << '\n';
		SDL_Quit();
		exit(1);
	}

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		SDL_GL_DestroyContext(context);
		SDL_DestroyWindow(window);
		std::cout << "GLEW Error: Failed to init" << '\n';
		SDL_Quit();
		exit(1);
	}

	// glEnable(GL_DEBUG_OUTPUT);
	// glDebugMessageCallback(MessageCallback, nullptr);

	// print GPU data
	std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << '\n';
	std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << '\n';
	std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << '\n';

	GLint total_mem_kb = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &total_mem_kb);
	std::cout << "GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX: " << total_mem_kb << '\n';
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_mem_kb);
	std::cout << "GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX: " << total_mem_kb << '\n';

	std::cout << '\n' << "Extensions: " << '\n';
	int num_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
	for (int i = 0; i < num_extensions; i++) {
		// std::cout << glGetStringi(GL_EXTENSIONS, i) << '\n';
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	init();

	SDL_Event event;
	bool quit = false;
	while (!quit) {
		SDL_Scancode active_scancode = SDL_SCANCODE_UNKNOWN;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT: quit = true; break;
				case SDL_EVENT_KEY_DOWN:
					if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
						quit = true;
					}
					active_scancode = event.key.scancode;
					break;
			}
		}
		handleKeyInput(active_scancode);
		display(window);
	}

	ResourceManager::instance().clear();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
