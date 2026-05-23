#include "SDL3/SDL_events.h"
#include "cs300/OGLDebug.h"
#include "mesh.hpp"
#include "object.hpp"
#include "shader.hpp"

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

static std::unique_ptr<Object> object;

// Use the new ResourceManager for all OpenGL resources

void init() {
	// Ensure the shader is loaded into the ResourceManager by its name
	ResourceManager::instance().getShader("default", "./data/shaders/default.vert", "./data/shaders/default.frag");

	object = std::make_unique<Object>("Suzanne", "./data/meshes/suzanne.obj", "default");
}

void cleanup() {
	ResourceManager::instance().clear();
}

void display(SDL_Window* window) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (object) {
		object->draw();
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

#if _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif

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

	init();

	SDL_Event event;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT: quit = true; break;
				case SDL_EVENT_KEY_DOWN:
					if (event.key.type == SDL_SCANCODE_ESCAPE) {
						quit = true;
					}
					break;
			}
		}

		display(window);
	}

	cleanup();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
