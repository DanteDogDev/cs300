#include "OGLDebug.h"
#include "shader.hpp"

#include <SDL3/SDL.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

static int winID;
static GLsizei WIDTH = 1280;
static GLsizei HEIGHT = 720;

struct Vertex {
	float pos[4];
	float color[3];
};

const Vertex vertexData[3] = {
  {  {0.75f, 0.75f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
  { {0.75f, -0.75f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
  {{-0.75f, -0.75f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}
};

namespace {
std::unique_ptr<Shader> default_shader;
GLuint vertexBufferObject;
GLuint vao;
}

void InitializeProgram() {
	default_shader = std::make_unique<Shader>(Shader::makeDefault());
}

void InitializeBuffers() {
	// VAO
	glGenVertexArrays(1, &vao);

	// VBO
	glGenBuffers(1, &vertexBufferObject);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// Insert the VBO into the VAO
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init() {
	InitializeProgram();
	InitializeBuffers();
}

// Called to update the display.
// You should call SDL_GL_SwapWindow after all of your rendering to display what you rendered.
void display(SDL_Window* window) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Bind the glsl program and this object's VAO
	default_shader->bind();
	glBindVertexArray(vao);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Unbind
	glBindVertexArray(0);
	default_shader->unbind();

	SDL_GL_SwapWindow(window);
}

void cleanup() {
	// Delete the program
	default_shader = nullptr;
	// Delete the VBOs
	glDeleteBuffers(1, &vertexBufferObject);
	// Delete the VAO
	glDeleteVertexArrays(1, &vao);
}

int main(int argc, char* args[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_Window* window = SDL_CreateWindow("CS300", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext context_ = SDL_GL_CreateContext(window);
	if (context_ == nullptr) {
		SDL_DestroyWindow(window);
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		SDL_GL_DestroyContext(context_);
		SDL_DestroyWindow(window);
		std::cout << "GLEW Error: Failed to init" << std::endl;
		SDL_Quit();
		exit(1);
	}

#if _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif

	// print GPU data
	std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;

	GLint totalMemKb = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &totalMemKb);
	std::cout << "GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX: " << totalMemKb << std::endl;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemKb);
	std::cout << "GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX: " << totalMemKb << std::endl;

	std::cout << std::endl << "Extensions: " << std::endl;
	int numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	for (int i = 0; i < numExtensions; i++) {
		std::cout << glGetStringi(GL_EXTENSIONS, i) << std::endl;
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

	SDL_GL_DestroyContext(context_);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
