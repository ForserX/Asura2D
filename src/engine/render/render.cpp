#include "pch.h"

using namespace Asura;

stl::hash_map<ResourcesManager::id_t, ImTextureID> textures_list;

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

	textures_list.clear();

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

	OPTICK_EVENT("Graphics present");

	glClearColor(clear_color[0] * 255, clear_color[1] * 255, clear_color[2] * 255, clear_color[3] * 255);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Render::texture_id Render::GetTexture(ResourcesManager::id_t resource_id)
{
    return LoadTexture(resource_id);
}

Render::texture_id Render::LoadTexture(ResourcesManager::id_t resource_id)
{
	static uint32_t DTexture = 0;

	if (textures_list.contains(resource_id))
	{
		return textures_list.at(resource_id);
	}

	Texture ImageBytes = {};

	Resource CurrentTexture = ResourcesManager::GetResource(resource_id);
	FileSystem::Path Path = FileSystem::ContentDir() / CurrentTexture.Name;

	ImageBytes.Bytes = SOIL_load_image(Path.generic_string().c_str(), &ImageBytes.Width, &ImageBytes.Height, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &DTexture);
	glBindTexture(GL_TEXTURE_2D, DTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageBytes.Width, ImageBytes.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageBytes.Bytes);
	SOIL_free_image_data(ImageBytes.Bytes);

	glBindTexture(GL_TEXTURE_2D, 0);

	ImTextureID texture_handle = (ImTextureID)(intptr_t)DTexture;
	textures_list[resource_id] = texture_handle;

	return texture_handle;
}