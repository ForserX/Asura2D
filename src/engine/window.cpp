#include <SDL_syswm.h>

#include "pch.h"

SDL_Window* window_handle = nullptr;
bool wants_to_exit = false;
bool fullscreen_mode = false;
extern bool show_console;
int window_width = 1024;
int window_height = 614;

using namespace ark;

void
window::init()
{
	auto window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN);

	if (fullscreen_mode) {
		window_flags = static_cast<SDL_WindowFlags>(window_flags | SDL_WINDOW_FULLSCREEN);
	}

	// Setup window
	window_handle = SDL_CreateWindow("Arkane", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, window_flags);

#ifdef ARK_VULKAN
	graphics::init_vulkan();
#endif
}

void
window::destroy()
{
	SDL_DestroyWindow(window_handle);
}

void
window::tick()
{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	SDL_Event event = {};
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			wants_to_exit = true;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.windowID == SDL_GetWindowID(window_handle)) {
				switch (event.window.event) 
				{
				case SDL_WINDOWEVENT_CLOSE:
					wants_to_exit = true;
					break;
				case SDL_WINDOWEVENT_RESIZED:
					break;
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					window_width = event.window.data1;
					window_height = event.window.data2;

					camera::reset_wh();
					break;
				default:
					break;
				}
			}
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_GRAVE) {
				show_console = !show_console;
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
				camera::move(camera::cam_move::right, 0.5f);
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
				camera::move(camera::cam_move::left, 0.5f);
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
				camera::move(camera::cam_move::up, 0.5f);
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
				camera::move(camera::cam_move::down, 0.5f);
			}
			break;
		}
		default:
			break;
		}
	}

	engine::tick();
}

void
window::change_fullscreen()
{
	SDL_SetWindowFullscreen(window_handle, fullscreen_mode ? SDL_WINDOW_FULLSCREEN : 0);
}

void
window::change_resolution()
{
	SDL_SetWindowSize(window_handle, window_width, window_height);
}

void
window::loop()
{
	while (!wants_to_exit) {
		tick();
	}
}
