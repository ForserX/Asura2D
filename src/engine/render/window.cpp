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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Setup window
	window_handle = glfwCreateWindow(window_width, window_height, "Asura 2D", fullscreen_mode ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	glfwMakeContextCurrent(window_handle);

	if (!gladLoadGL())
	{
		Debug::Msg("GLAD Load error");
		game_assert(false, "glad init error!", {});
	}

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

	engine::Tick();

	glfwSwapBuffers(window_handle);
}

void Window::change_fullscreen()
{
	const GLFWvidmode* Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	if (!fullscreen_mode)
	{
		glfwSetWindowMonitor(window_handle, nullptr, 100, 100, window_width, window_height, Mode->refreshRate);

		int MonitorX, MonitorY;
		GLFWmonitor* Monitors = glfwGetPrimaryMonitor();
		glfwGetMonitorPos(Monitors, &MonitorX, &MonitorY);
		const GLFWvidmode* videoMode = glfwGetVideoMode(Monitors);

		glfwSetWindowPos(window_handle,
			MonitorX + (videoMode->width - window_width) / 2,
			MonitorY + (videoMode->height - window_height) / 2);

		return;
	}
	
	glfwSetWindowMonitor(window_handle, glfwGetPrimaryMonitor(), 100, 100, Mode->width, Mode->height, Mode->refreshRate);
}

void Window::change_window_mode()
{
	if (window_maximized) 
	{
		glfwMaximizeWindow(window_handle);
	}
	else if (glfwGetWindowAttrib(window_handle, GLFW_MAXIMIZED))
	{
		glfwRestoreWindow(window_handle);
	}
}

void Window::change_resolution()
{
	glfwSetWindowSize(window_handle, window_width, window_height);
	Camera::ResetHW();
	Camera::ResetView();
	glViewport(0, 0, window_width, window_height);
}

void Window::loop()
{
	while (!wants_to_exit) 
	{
		Tick();
	}
}
