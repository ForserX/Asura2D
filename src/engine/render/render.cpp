#include "pch.h"
#include "shaders/tex_rotation.h"
#include "shaders/tex_mirror.h"

using namespace Asura;

stl::hash_map<ResourcesManager::id_t, ImTextureID> TextureList;

GLuint RenderVBODefault;
GLuint RenderVAODefault;

// Shaders
stl::vector<Render::RenderData> RenderList;
Render::Shaders::TextureRotation* TexRotate = nullptr;
Render::Shaders::TextureMirror* TexMirror = nullptr;

void Render::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	game_assert(ImGui::GetCurrentContext() != nullptr, "ImGui Context is broken", std::terminate());

	const ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(window_handle, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	Graphics::Init();
	Graphics::theme::change();

	TexMirror = new Render::Shaders::TextureMirror;
	TexMirror->Build();

	TexRotate = new Render::Shaders::TextureRotation;
	TexRotate->Build();

	Console::MakeConsoleCommand<Console::CommandBoolean>("draw_fps", &show_fps_counter);

	class CommandUIColor : Console::CommandTemplate
	{
	public:
		CommandUIColor(stl::string Name) :
			Console::CommandTemplate(Name)
		{}

		virtual void SetupHint() override { Hint = "red/dark/white"; };
		virtual void Exec(stl::string_view Command) override
		{
			if (Command == "red")
			{
				window_style = Graphics::theme::style::red;
			}
			else if (Command == "dark")
			{
				window_style = Graphics::theme::style::dark;
			}
			else if (Command == "white")
			{
				window_style = Graphics::theme::style::white;
			}
			else 
			{
				window_style = Graphics::theme::style::invalid;
			}

			Graphics::theme::change();
		}
	};

	Console::MakeConsoleCommand<CommandUIColor>("ui_color");
}

void Render::Destroy()
{
	Graphics::Destroy();

	TextureList.clear();

	glDeleteBuffers(1, &RenderVBODefault);
	glDeleteVertexArrays(1, &RenderVAODefault);
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete TexRotate;
	delete TexMirror;
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

	for (const auto &Data : RenderList)
	{
		if (Data.MirrorX)
		{
			TexMirror->Render(Data);
		}
		else
		{
			TexRotate->Render(Data);
		}
	}

	RenderList.clear();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Asura::Render::Push(RenderData Data)
{
	RenderList.emplace_back(std::move(Data));
}

Render::texture_id Render::GetTexture(ResourcesManager::id_t resource_id)
{
	return LoadTexture(resource_id);
}

Render::texture_id Render::LoadTexture(ResourcesManager::id_t resource_id)
{
	static uint32_t DTexture = 0;

	if (TextureList.contains(resource_id))
	{
		return TextureList.at(resource_id);
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
	TextureList[resource_id] = texture_handle;

	return texture_handle;
}
