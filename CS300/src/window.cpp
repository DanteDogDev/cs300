#include "window.hpp"

Window::Window(const char* title, GLsizei width, GLsizei height)
    : m_window(nullptr),
      m_context(nullptr),
      m_title(title),
      m_width(width),
      m_height(height) { }

Window::~Window() {
	shutdown();
}

auto Window::init() -> bool {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return false;
	}

	m_window = SDL_CreateWindow(m_title, m_width, m_height, SDL_WINDOW_OPENGL);
	if (m_window == nullptr) {
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	m_context = SDL_GL_CreateContext(m_window);
	if (m_context == nullptr) {
		SDL_DestroyWindow(m_window);
		return false;
	}

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		SDL_GL_DestroyContext(m_context);
		SDL_DestroyWindow(m_window);
		return false;
	}

	return true;
}

void Window::swapBuffers() {
	SDL_GL_SwapWindow(m_window);
}

void Window::shutdown() {
	if (m_context) {
		SDL_GL_DestroyContext(m_context);
		m_context = nullptr;
	}
	if (m_window) {
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
	SDL_Quit();
}

auto Window::processEvents(SDL_Event& event, SDL_Scancode& active_scancode) -> bool {
	bool quit = false;
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
	return quit;
}
