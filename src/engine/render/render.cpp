#include "pch.h"
#include <GLFW/glfw3.h>

using namespace Asura;

stl::hash_map<ResourcesManager::id_t, ImTextureID> textures_list;


GLuint RenderVBODefault, RenderVAODefault;

// Shaders
stl::vector<Render::RenderData> RenderList;
Render::Shaders::ShaderProgram* TextureShader;

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

	TextureShader = new Shaders::ShaderProgram("image_rotation_ps.glsl", "image_rotation_vs.glsl");

	if (TextureShader->Build())
	{
		TextureShader->Link();
		TextureShader->FreeBuildData();

		// Set up vertex data (and buffer(s)) and attribute pointers
		GLfloat vertices[] =
		{
			 -1.f, -1.f, 0.0f, // Left  
			 -1.f,  1.f, 0.0f, // Top   
			 1.f, -1.f, 0.0f, // Right 


			 1.f, -1.f, 0.0f, // Right 
			 -1.f,  1.f, 0.0f, // Top   
			 1.f, 1.f, 0.0f, // Right 
		};

		glGenVertexArrays(1, &RenderVAODefault);
		glGenBuffers(1, &RenderVBODefault);

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(RenderVAODefault);

		glBindBuffer(GL_ARRAY_BUFFER, RenderVBODefault);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}
}

void Render::Destroy()
{
	Graphics::Destroy();

	textures_list.clear();

	glDeleteBuffers(1, &RenderVBODefault);
	glDeleteVertexArrays(1, &RenderVAODefault);
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete(TextureShader);
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

	auto RenderFrame = [dt](float ScaleX, float ScaleY, float PosX, float PosY, float Angle, GLuint Texture)
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		auto RenderProgram = TextureShader->Use();
		unsigned int ScreenMatrix = glGetUniformLocation(RenderProgram, "ScreenMatrix");
		float L = 0;
		float R = 0 + window_width;
		float T = 0;
		float B = 0 + window_height;
		const float ortho_projection[4][4] =
		{
			{ 2.0f / (R - L),   0.0f,         0.0f,   0.0f },
			{ 0.0f,         2.0f / (T - B),   0.0f,   0.0f },
			{ 0.0f,         0.0f,        -1.0f,   0.0f },
			{ (R + L) / (L - R),  (T + B) / (B - T),  0.0f,   1.0f },
		};

		glUniformMatrix4fv(ScreenMatrix, 1, GL_FALSE, &ortho_projection[0][0]);

		unsigned int WorldMatrixID = glGetUniformLocation(RenderProgram, "WorldMatrix");

		const float WorldMatrix1[4][4] =
		{
			{ScaleX,0,0,0 },
			{0,ScaleY,0,0 },
			{0,0,1,0 },
			{PosX,PosY,0,1 },
		};

		//Angle *= M_PI;
		const float WorldMatrix2[4][4] =
		{
			{cosf(Angle),-sinf(Angle),0,0},
			{sinf(Angle),cosf(Angle),0,0 },
			{0,0,1,0 },
			{0,0,0,1 },
		};
		float ResultTest[4][4] = {};

		Math::Multiply4x4(&WorldMatrix2[0][0], &WorldMatrix1[0][0], &ResultTest[0][0]);

		glUniformMatrix4fv(WorldMatrixID, 1, GL_FALSE, &ResultTest[0][0]);

		glBindVertexArray(RenderVAODefault);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(glGetUniformLocation(RenderProgram, "InTexture"), 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glDisable(GL_BLEND);
	};

	for (const auto &Data : RenderList)
	{
		RenderFrame(Data.ScaleX, Data.ScaleY, Data.x, Data.y, Data.Angle, Data.TextureID);
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
