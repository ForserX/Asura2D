#include "pch.h"

extern SDL_Window* window_handle;
SDL_Renderer* renderer = nullptr;

using namespace ark;

void 
graphics::pre_init()
{
	std::string render_list = "";
#if defined(_WIN32)
	std::string mode = "direct3d11";
#elif defined(__linux__)
	std::string mode = "opengl";
#elif defined(__ANDROID__)
	std::string mode = "opengles2";
#endif

	for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
	{
		SDL_RendererInfo rendererInfo = {};
		SDL_GetRenderDriverInfo(i, &rendererInfo);
#ifdef ARK_DX12
		if (rendererInfo.name == std::string("direct3d12")) {
			mode = rendererInfo.name;
		}
#endif
		render_list += rendererInfo.name;
		render_list += ", ";
	}

	debug::msg("SDL Render mode support: {}", render_list);

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, mode.c_str());
}

void
graphics::init()
{
	pre_init();

	renderer = SDL_CreateRenderer(window_handle, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	ark_assert(renderer != nullptr, "Error creating SDL_Renderer!", return);
	
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	debug::msg("Current SDL_Renderer: {}", info.name);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ark_assert(ImGui::GetCurrentContext() != nullptr, "ImGui Context is broken", std::terminate());

	const ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window_handle, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
}

void
graphics::init_vulkan()
{
#ifdef ARK_VULKAN
	SDL_Vulkan_LoadLibrary(nullptr);
	uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(window_handle, &extensionCount, nullptr);

	const char** extensionNames = new const char* [extensionCount];
	SDL_Vulkan_GetInstanceExtensions(window_handle, &extensionCount, extensionNames);
#endif
}

void
graphics::destroy()
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
}

void
graphics::tick()
{
	static float clear_color[] = { 0.f, 0.f, 0.f, 1.f };
	
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	const auto& io = ImGui::GetIO();
	ui::tick(io.DeltaTime);
	
	// Rendering
	ImGui::Render();
	SDL_SetRenderDrawColor(
		renderer,
		static_cast<Uint8>(clear_color[0] * 255),
		static_cast<Uint8>(clear_color[1] * 255),
		static_cast<Uint8>(clear_color[2] * 255),
		static_cast<Uint8>(clear_color[3] * 255)
	);

	SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer);
}
