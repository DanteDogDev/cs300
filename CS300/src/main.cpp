#include "cs300/OGLDebug.h"
#include "window.hpp"
#include "world.hpp"

#include <SDL3/SDL.h>

const GLsizei initial_width = 1280;
const GLsizei initial_height = 720;
const int target_fps = 60;
const float target_frame_time_ms = 1000.f / target_fps;

auto main(int argc, char* args[]) -> int {
	Window window("CS300", initial_width, initial_height);
	if (!window.init()) {
		return 1;
	}

	World world(initial_width, initial_height);
	world.init();

	printf("Starting Update Loop\n");
	float current_time_seconds = 0.0f;
	unsigned int frame_start_time_ms = 0;
	unsigned int frame_end_time_ms = 0;
	unsigned int elapsed_time_ms = 0;

	SDL_Event event;
	SDL_Scancode active_scancode = SDL_SCANCODE_UNKNOWN;
	bool quit = false;
	while (!quit) {
		frame_start_time_ms = SDL_GetTicks();
		current_time_seconds = frame_start_time_ms / 1000.f;

		quit = window.processEvents(event, active_scancode);
		world.handleKeyInput(active_scancode);

		world.render(window);
		window.swapBuffers();

		world.update(current_time_seconds);

		frame_end_time_ms = SDL_GetTicks();
		elapsed_time_ms = frame_end_time_ms - frame_start_time_ms;

		if (elapsed_time_ms < target_frame_time_ms) {
			SDL_Delay(static_cast<unsigned int>(target_frame_time_ms - elapsed_time_ms));
		}
	}

	world.clear();
	window.shutdown();

	return 0;
}
