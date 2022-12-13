#include "pch.h"
#include <SDL_syswm.h>

SDL_Window* window_handle = nullptr;
bool wants_to_exit = false;
bool fullscreen_mode = false;

int window_width = 1600;
int window_height = 900;
bool window_maximized = false;

using namespace Asura;
using namespace Asura::GamePlay;

void window::Init()
{
	auto window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE);

	if (fullscreen_mode) 
	{
		window_flags = static_cast<SDL_WindowFlags>(window_flags | SDL_WINDOW_FULLSCREEN);
	}

	// Setup window
	// #TODO: Set as main.cpp
	window_handle = SDL_CreateWindow("Asura 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, window_flags);

#ifdef ASURA_VULKAN
	Graphics::init_vulkan();
#endif
}

void window::Destroy()
{
	wants_to_exit = true;
	SDL_DestroyWindow(window_handle);
}

bool window::IsDestroyed()
{
	return wants_to_exit;
}

void window::Tick()
{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse Input data to your main application, or clear/overwrite your copy of the mouse data.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard Input data to your main application, or clear/overwrite your copy of the keyboard data.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	SDL_Event event = {};
	while (SDL_PollEvent(&event))
	{
		int tx = 0;
		int ty = 0;
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type) 
		{
		case SDL_QUIT:
			wants_to_exit = true;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.windowID == SDL_GetWindowID(window_handle)) 
			{
				switch (event.window.event) 
				{
				case SDL_WINDOWEVENT_CLOSE:
					wants_to_exit = true;
					break;
				case SDL_WINDOWEVENT_RESIZED:
					window_width = event.window.data1;
					window_height = event.window.data2;
					Camera::ResetHW();
#ifdef OS_WINDOWS
					// Stupid bug on Windows
					SDL_GetWindowPosition(window_handle, &tx, &ty);
					SDL_SetWindowPosition(window_handle, tx + 1, ty + 1);
					SDL_SetWindowPosition(window_handle, tx, ty);
#endif
					break;
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					window_width = event.window.data1;
					window_height = event.window.data2;
					Camera::ResetHW();
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
		case SDL_MOUSEMOTION: 
		{
			const auto pos = ImGui::GetMousePos();
			Input::UpdateMousePos({static_cast<short>(pos.x), static_cast<short>(pos.y)});
		}
		break;
		case SDL_KEYDOWN:
			Input::UpdateKey(event.key.keysym.scancode, 1.f);
			break;
		case SDL_KEYUP:
			Input::UpdateKey(event.key.keysym.scancode, 0.f);
			break;
		case SDL_MOUSEBUTTONDOWN:
			Input::UpdateKey(SDL_SCANCODE_ENDCALL + static_cast<int16_t>(event.button.button), 1.f);
			break;
		case SDL_MOUSEBUTTONUP:
			Input::UpdateKey(SDL_SCANCODE_ENDCALL + static_cast<int16_t>(event.button.button), 0.f);
			break;
		case SDL_MOUSEWHEEL:
			Input::UpdateKey(SDL_SCANCODE_MOUSEWHEEL, event.wheel.y);
			break;
		default:
			break;
		}
	}

	engine::Tick();
}

void window::change_fullscreen()
{
	SDL_SetWindowFullscreen(window_handle, fullscreen_mode ? SDL_WINDOW_FULLSCREEN : 0);
}

void window::change_window_mode()
{
	if (window_maximized) 
	{
		SDL_MaximizeWindow(window_handle);
	}
	else if (SDL_GetWindowFlags(window_handle) & SDL_WINDOW_MAXIMIZED)
	{
		SDL_RestoreWindow(window_handle);
	}
}

void window::change_resolution()
{
	SDL_SetWindowSize(window_handle, window_width, window_height);
	Camera::ResetHW();
	Camera::ResetView();
}

void window::loop()
{
	while (!wants_to_exit) 
	{
		Tick();
	}
}
