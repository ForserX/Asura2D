#include "arkane.h"

SDL_Window* window_handle = nullptr;
bool wants_to_exit = false;

using namespace ark;

void
window::init()
{
	const int64_t width = core::get_cmd_int("window_width");
	const int64_t height = core::get_cmd_int("window_height");

	// Setup window
	constexpr SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	window_handle = SDL_CreateWindow("Arkane", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
}

void
window::destroy()
{
	SDL_DestroyWindow(window_handle);
}

void
window::tick(float dt)
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
			if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window_handle)) {
				wants_to_exit = true;
			}
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			//ImGui_ImplSDL2_UpdateKeyModifiers((SDL_Keymod)event->key.keysym.mod);
			//ImGuiKey key = ImGui_ImplSDL2_KeycodeToImGuiKey(event->key.keysym.sym);
			//io.AddKeyEvent(key, (event->type == SDL_KEYDOWN));
			//io.SetKeyEventNativeData(key, event->key.keysym.sym, event->key.keysym.scancode, event->key.keysym.scancode); // To support legacy indexing (<1.87 user code). Legacy backend uses SDLK_*** as indices to IsKeyXXX() functions.
		}
		default:
			break;
		}
	}

	graphics::tick(dt);
}

void
window::loop()
{
	auto first_time = std::chrono::high_resolution_clock::now();
	auto second_time = std::chrono::high_resolution_clock::now();
	
	while (!wants_to_exit) {
		first_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> difference_time = second_time - first_time;
		tick(difference_time.count());
		second_time = std::chrono::high_resolution_clock::now();
	}
}
