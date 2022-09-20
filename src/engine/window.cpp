﻿#include <SDL_syswm.h>

#include "pch.h"

SDL_Window* window_handle = nullptr;
bool wants_to_exit = false;

using namespace ark;

void
window::init()
{
	auto window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN);

	if (core::get_cmd_int("window_fullscreen")) {
		window_flags = static_cast<SDL_WindowFlags>(window_flags | SDL_WINDOW_FULLSCREEN);
	}

	int width  = static_cast<int>(core::get_cmd_int("window_width"));
	int height = static_cast<int>(core::get_cmd_int("window_height"));

	if (width < 1024)
	{
		width = 1024;
		height = 614;
	}

	// Setup window
	window_handle = SDL_CreateWindow("Arkane", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);

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
				switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					wants_to_exit = true;
					break;
				case SDL_WINDOWEVENT_RESIZED:
					break;
				default:
					break;
				}
				if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
					wants_to_exit = true;
				}
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

	graphics::tick();
}

void
window::loop()
{
	while (!wants_to_exit) {
		tick();
	}
}