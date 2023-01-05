#include "pch.h"

using namespace Asura;

stl::hash_map<ResourcesManager::id_t, Render::texture_id> textures_list;

void Render::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	game_assert(ImGui::GetCurrentContext() != nullptr, "ImGui Context is broken", std::terminate());

	const ImGuiIO& io = ImGui::GetIO();
	(void)io;
	
	ImGui_ImplGlfw_InitForOpenGL(window_handle, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	Graphics::Init();
	Graphics::theme::change();
}

void Render::Destroy()
{
	Graphics::Destroy();
#if 0
    for (auto [resource, texture] : textures_list)
	{
        SDL_DestroyTexture(static_cast<SDL_Texture*>(texture));
    }
#endif

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Render::Tick(float dt)
{
	OPTICK_EVENT("Render Destroy");
#ifdef OPTICK_ENABLED
	OPTICK_CATEGORY("Systems Destroy", Optick::Category::Rendering);
#endif

	static float clear_color[] = { 0.f, 0.f, 0.f, 1.f };

	{
		OPTICK_EVENT("Render new frame prepare");
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	{
		OPTICK_EVENT("Graphics Destroy");
		Graphics::Tick(dt);
	}

	{
		OPTICK_EVENT("ImGui Render");
		
		// Rendering
		ImGui::Render();
	}
#if 0
	{
		OPTICK_EVENT("Graphics present");
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
#endif
	OPTICK_EVENT("Graphics present");

	glClearColor(clear_color[0] * 255, clear_color[1] * 255, clear_color[2] * 255, clear_color[3] * 255);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
#if 0
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
#else
	return nullptr;
#endif
}