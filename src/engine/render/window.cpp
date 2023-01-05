#include "pch.h"

GLFWwindow* window_handle = nullptr;
bool wants_to_exit = false;
bool fullscreen_mode = false;

int window_width = 1600;
int window_height = 900;
bool window_maximized = false;

using namespace Asura;
using namespace Asura::GamePlay;

namespace Asura::Window::Internal
{
	void RegCallbacks()
	{
		glfwSetWindowCloseCallback(window_handle,
			[](GLFWwindow*)
			{
				wants_to_exit = true;
				glfwSetWindowShouldClose(window_handle, GLFW_FALSE);
			}
		);

		glfwSetWindowSizeCallback(window_handle,
			[](GLFWwindow* window, int width, int height)
			{
				window_width = width;
				window_height = height;
				Camera::ResetHW();
				glViewport(0, 0, width, height);
			}
		);

		glfwSetKeyCallback(window_handle,
			[](GLFWwindow* window, int key, int scancode, int action, int mode)
			{
				if (action == GLFW_PRESS)
					Input::UpdateKey((int16_t)key, 1.f);

				if (action == GLFW_RELEASE)
					Input::UpdateKey((int16_t)key, 0.f);
			}
		);

		glfwSetMouseButtonCallback(window_handle,
			[](GLFWwindow* window, int button, int action, int mods)
			{
				if (action == GLFW_PRESS)
					Input::UpdateKey((int16_t)button, 1.f);

				if (action == GLFW_RELEASE)
					Input::UpdateKey((int16_t)button, 0.f);
			}
		);

		glfwSetCursorPosCallback(window_handle,
			[](GLFWwindow* window, double xpos, double ypos)
			{
				//const auto pos = ImGui::GetMousePos();
				Input::UpdateMousePos({ static_cast<short>(xpos), static_cast<short>(ypos) });
			}
		);

		glfwSetScrollCallback(window_handle, 
			[](GLFWwindow* window, double xoffset, double yoffset)
			{
				Input::UpdateKey(GLFW_MOUSE_BUTTON_MIDDLE, yoffset);
			}
		);
	}
}

void Window::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#if 0
	auto window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE);

	if (fullscreen_mode) 
	{
		window_flags = static_cast<SDL_WindowFlags>(window_flags | SDL_WINDOW_FULLSCREEN);
	}

#endif
	// Setup window
	// #TODO: Set as main.cpp
	window_handle = glfwCreateWindow(window_width, window_height, "Asura 2D", nullptr, nullptr);
	glfwMakeContextCurrent(window_handle);
	glViewport(0, 0, window_width, window_height);

	Internal::RegCallbacks();
}

void Window::Destroy()
{
	wants_to_exit = true;
	glfwTerminate();
}

bool Window::IsDestroyed()
{
	return wants_to_exit;
}

void Window::Tick()
{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse Input data to your main application, or clear/overwrite your copy of the mouse data.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard Input data to your main application, or clear/overwrite your copy of the keyboard data.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

	glfwPollEvents();
	glfwSwapBuffers(window_handle);

	engine::Tick();
}

void Window::change_fullscreen()
{
#if 0
	SDL_SetWindowFullscreen(window_handle, fullscreen_mode ? SDL_WINDOW_FULLSCREEN : 0);
#endif
}

void Window::change_window_mode()
{
#if 0
	if (window_maximized) 
	{
		SDL_MaximizeWindow(window_handle);
	}
	else if (SDL_GetWindowFlags(window_handle) & SDL_WINDOW_MAXIMIZED)
	{
		SDL_RestoreWindow(window_handle);
	}
#endif
}

void Window::change_resolution()
{
	glfwSetWindowSize(window_handle, window_width, window_height);
	Camera::ResetHW();
	Camera::ResetView();
}

void Window::loop()
{
	while (!wants_to_exit) 
	{
		Tick();
	}
}
