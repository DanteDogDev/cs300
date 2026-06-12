#pragma once
#include <GL/glew.h>
#include <SDL3/SDL.h>

class Window {
public:
	Window(const char* title, GLsizei width, GLsizei height);
	~Window();

	auto init() -> bool;
	void swapBuffers();
	void shutdown();
	auto processEvents(SDL_Event& event, SDL_Scancode& active_scancode) -> bool;

	[[nodiscard]]
	auto getWidth() const -> GLsizei {
		return m_width;
	}

	[[nodiscard]]
	auto getHeight() const -> GLsizei {
		return m_height;
	}

	[[nodiscard]]
	auto getSDLWindow() const -> SDL_Window* {
		return m_window;
	}

private:
	SDL_Window* m_window;
	SDL_GLContext m_context;
	const char* m_title;
	GLsizei m_width;
	GLsizei m_height;
};
