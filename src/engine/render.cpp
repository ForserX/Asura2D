#include "pch.h"

extern SDL_Window* window_handle;
SDL_Renderer* renderer = nullptr;
extern int window_height;

using namespace ark;

using texture_id = ImTextureID;
stl::hash_map<int32_t, texture_id> textures_list;

void 
render::pre_init()
{
	stl::string render_list;
#if defined(_WIN32)
	stl::string mode = "direct3d11";
#elif defined(__linux__)
	stl::string mode = "opengl";
#elif defined(__ANDROID__)
	stl::string mode = "opengles2";
#elif defined(__APPLE__)
    stl::string mode = "metal";
#else
	stl::string mode = "opengl";
#endif

	for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
	{
		SDL_RendererInfo rendererInfo = {};
		SDL_GetRenderDriverInfo(i, &rendererInfo);
#ifdef ARK_DX12
		if (!stl::string_view("direct3d12").compare(rendererInfo.name)) {
			mode = rendererInfo.name;
		}
#endif
		render_list += rendererInfo.name;
		render_list += ", ";
	}

	debug::msg("SDL Render mode support: {}", render_list);

    SDL_setenv("METAL_DEVICE_WRAPPER_TYPE", "1", 0);
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, mode.c_str());
}

void
render::init()
{
	renderer = SDL_CreateRenderer(window_handle, -1, SDL_RENDERER_PRESENTVSYNC);

	ark_assert(renderer != nullptr, "Error creating SDL_Renderer!", return);
	
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	debug::msg("Current SDL_Renderer: {}", info.name);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ark_assert(ImGui::GetCurrentContext() != nullptr, "ImGui Context is broken", std::terminate());

	const ImGuiIO& io = ImGui::GetIO();
	(void)io;
	
	ImGui_ImplSDL2_InitForSDLRenderer(window_handle, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	graphics::init();
}

void
render::init_vulkan()
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
render::destroy()
{
	graphics::destroy();
    
    for (auto [resource, texture] : textures_list) {
        SDL_DestroyTexture(static_cast<SDL_Texture*>(texture));
    }
	
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
}

void
render::tick(float dt)
{
	OPTICK_EVENT("render tick")
	OPTICK_CATEGORY("systems tick", Optick::Category::Rendering)
	static float clear_color[] = { 0.f, 0.f, 0.f, 1.f };

	{
		OPTICK_EVENT("render new frame prepare")
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	{
		OPTICK_EVENT("graphics tick")
		graphics::tick(dt);
	}

	{
		OPTICK_EVENT("ImGui render")
		
		// Rendering
		ImGui::Render();
	}

	{
		OPTICK_EVENT("graphics present")
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

ImTextureID
render::get_texture(int32_t resource_id)
{
    if (!textures_list.contains(resource_id)) {
        return nullptr;
    }
    
    return textures_list.at(resource_id);
}

ImTextureID
render::load_texture(int32_t resource_id)
{
    if (textures_list.contains(resource_id)) {
        return textures_list.at(resource_id);
    }
    
    SDL_RWops* rw = SDL_RWFromConstMem(resources::ptr(resource_id), resources::size(resource_id));
    if (rw == nullptr) {
        return nullptr;
    }
    
    resources::lock(resource_id);
    ImTextureID texture_handle = IMG_LoadTexture_RW(renderer, rw, 0);
    resources::unlock(resource_id);
    
    if (texture_handle == nullptr) {
        return nullptr;
    }
    
    textures_list[resource_id] = texture_handle;
    return texture_handle;
}
