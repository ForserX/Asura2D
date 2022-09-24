#include <SDL_syswm.h>

#include "pch.h"

SDL_Window* window_handle = nullptr;
bool wants_to_exit = false;
bool fullscreen_mode = false;
extern bool show_console;

int window_width = 1024;
int window_height = 614;
bool window_maximized = false;

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
	wants_to_exit = true;
	SDL_DestroyWindow(window_handle);
}

bool
window::is_destroyed()
{
	return wants_to_exit;
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
					window_width = event.window.data1;
					window_height = event.window.data2;

					camera::reset_wh();
					break;
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					window_width = event.window.data1;
					window_height = event.window.data2;
					camera::reset_wh();
					break;
				case SDL_WINDOWEVENT_RESTORED:
					window_maximized = false;
					break;
				case SDL_WINDOWEVENT_MAXIMIZED:
					window_maximized = true;
					break;
				default:
					break;
				}
			}
			break;
		case SDL_MOUSEMOTION: {
			const ark_float_vec2 pos = ImGui::GetMousePos();
			input::update_mouse_pos({static_cast<short>(pos.x), static_cast<short>(pos.y)});
		}
		break;
		case SDL_KEYDOWN:
			input::update_key(event.key.keysym.scancode, 1.f);
			break;
		case SDL_KEYUP:
			input::update_key(event.key.keysym.scancode, 0.f);
			break;
		case SDL_MOUSEBUTTONDOWN:
			input::update_key(SDL_SCANCODE_ENDCALL + static_cast<int16_t>(event.button.button), 1.f);
			break;
		case SDL_MOUSEBUTTONUP:
			input::update_key(SDL_SCANCODE_ENDCALL + static_cast<int16_t>(event.button.button), 0.f);
			break;
		case SDL_MOUSEWHEEL:
			input::update_key(SDL_SCANCODE_MOUSEWHEEL, event.wheel.y);
			break;
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
window::change_window_mode()
{
	if (window_maximized) {
		SDL_MaximizeWindow(window_handle);
	}
	else if (SDL_GetWindowFlags(window_handle) & SDL_WINDOW_MAXIMIZED)
	{
		SDL_RestoreWindow(window_handle);
	}
}

void
window::change_resolution()
{
	SDL_SetWindowSize(window_handle, window_width, window_height);
	camera::reset_wh();
	camera::reset_view();
}

void
window::loop()
{
	while (!wants_to_exit) {
		tick();
	}
}
