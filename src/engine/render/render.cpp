#include "pch.h"

SDL_Renderer* renderer = nullptr;

using namespace Asura;

stl::hash_map<ResourcesManager::id_t, Render::texture_id> textures_list;

void Render::InitialSDLDevice()
{
	stl::string render_list;
#if defined(OS_WINDOWS)
	stl::string mode = "direct3d11";
#elif defined(OS_LINUX)
	stl::string mode = "opengl";
#elif defined(OS_ANDROID)
	stl::string mode = "opengles2";
#elif defined(OS_APPLE_SERIES)
    stl::string mode = "metal";
#else
	stl::string mode = "opengl";
#endif

	for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
	{
		SDL_RendererInfo rendererInfo = {};
		SDL_GetRenderDriverInfo(i, &rendererInfo);
#ifdef ASURA_DX12
		if (!stl::string_view("direct3d12").compare(rendererInfo.name))
		{
			mode = rendererInfo.name;
		}
#endif
		render_list += rendererInfo.name;
		render_list += ", ";
	}

	Debug::msg("SDL Render mode support: {}", render_list);
#if defined(OS_APPLE_SERIES)
    SDL_setenv("METAL_DEVICE_WRAPPER_TYPE", "1", 0);
#endif
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, mode.c_str());
}

void Render::Init()
{
	InitialSDLDevice();

	renderer = SDL_CreateRenderer(window_handle, -1, SDL_RENDERER_PRESENTVSYNC);

	game_assert(renderer != nullptr, "Error creating SDL_Renderer!", return);
	
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	Debug::msg("Current SDL_Renderer: {}", info.name);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	game_assert(ImGui::GetCurrentContext() != nullptr, "ImGui Context is broken", std::terminate());

	const ImGuiIO& io = ImGui::GetIO();
	(void)io;
	
#if defined(OS_WINDOWS) & defined(ASURA_DX12)
	ImGui_ImplSDL2_InitForD3D(window_handle);
#elif defined(OS_APPLE_SERIES)
	ImGui_ImplSDL2_InitForMetal(window_handle);
#else
	ImGui_ImplSDL2_InitForSDLRenderer(window_handle, renderer);
#endif

	ImGui_ImplSDLRenderer_Init(renderer);

	graphics::Init();
}

void Render::init_vulkan()
{
#ifdef ASURA_VULKAN
	SDL_Vulkan_LoadLibrary(nullptr);
	uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(window_handle, &extensionCount, nullptr);

	const char** extensionNames = new const char* [extensionCount];
	SDL_Vulkan_GetInstanceExtensions(window_handle, &extensionCount, extensionNames);
#endif
}

void Render::Destroy()
{
	graphics::Destroy();
    
    for (auto [resource, texture] : textures_list)
	{
        SDL_DestroyTexture(static_cast<SDL_Texture*>(texture));
    }
	
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
}

void Render::Tick(float dt)
{
	OPTICK_EVENT("Render Destroy")
	OPTICK_CATEGORY("Systems Destroy", Optick::Category::Rendering)
	static float clear_color[] = { 0.f, 0.f, 0.f, 1.f };

	{
		OPTICK_EVENT("Render new frame prepare");
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	{
		OPTICK_EVENT("graphics Destroy");
		graphics::Tick(dt);
	}

	{
		OPTICK_EVENT("ImGui Render");
		
		// Rendering
		ImGui::Render();
	}

	{
		OPTICK_EVENT("graphics present");
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
}

Render::texture_id Render::GetTexture(ResourcesManager::id_t resource_id)
{
    if (!textures_list.contains(resource_id)) 
	{
        return nullptr;
    }
    
    return textures_list.at(resource_id);
}

Render::texture_id Render::LoadTexture(ResourcesManager::id_t resource_id)
{
	if (textures_list.contains(resource_id))
	{
		return textures_list.at(resource_id);
	}

	Resource CurrentTexture = ResourcesManager::GetResource(resource_id);

	SDL_RWops* rw = SDL_RWFromConstMem(CurrentTexture.Ptr, CurrentTexture.Size);
	if (rw == nullptr)
	{
		return nullptr;
	}

	ResourceScopeLock Lock(CurrentTexture);
	ImTextureID texture_handle = IMG_LoadTexture_RW(renderer, rw, 0);

	if (texture_handle == nullptr)
	{
		return nullptr;
	}

	textures_list[resource_id] = texture_handle;
	return texture_handle;
}