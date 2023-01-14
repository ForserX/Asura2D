#include "pch.h"
#include <GLFW/glfw3.h>

using namespace Asura;

stl::hash_map<ResourcesManager::id_t, ImTextureID> textures_list;


GLuint RenderShaderProgramDefault;
GLuint RenderVBODefault, RenderVAODefault;

// Shaders
const GLchar* vertexShaderSource = 
"#version 330 core\n"
"uniform mat4 ScreenMatrix;"
"uniform mat4 WorldMatrix;"
"layout (location = 0) in vec3 position;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"gl_Position =ScreenMatrix*(WorldMatrix*vec4(position.x, position.y, position.z, 1.0));\n"
"TexCoord = (vec2(position.x, position.y)+vec2(1,1))*0.5f;\n"
"}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D InTexture;\n"
"void main()\n"
"{\n"
"color = texture(InTexture, TexCoord);\n"
"}\n\0";

stl::vector<Render::RenderData> RenderList;

void multiply(const float*mat1, const float*mat2,float*res)
{
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) 
		{
			res[i*4+j] = 0;
			for (k = 0; k < 4; k++)
				res[i*4+j] += mat1[i*4+k] * mat2[k*4+j];
		}
	}
}
void Render::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	game_assert(ImGui::GetCurrentContext() != nullptr, "ImGui Context is broken", std::terminate());

	const ImGuiIO& io = ImGui::GetIO();
	(void)io;
	
	ImGui_ImplGlfw_InitForOpenGL(window_handle, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	Graphics::Init();
	Graphics::theme::change();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		//std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		//std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	RenderShaderProgramDefault = glCreateProgram();
	glAttachShader(RenderShaderProgramDefault, vertexShader);
	glAttachShader(RenderShaderProgramDefault, fragmentShader);
	glLinkProgram(RenderShaderProgramDefault);
	// Check for linking errors
	glGetProgramiv(RenderShaderProgramDefault, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(RenderShaderProgramDefault, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

}

void Render::Destroy()
{
	Graphics::Destroy();

	textures_list.clear();

	glDeleteProgram(RenderShaderProgramDefault);
	glDeleteBuffers(1,&RenderVBODefault);
	glDeleteVertexArrays(1,&RenderVAODefault);
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

	auto RenderFrame = [dt](float Scale, float PosX, float PosY, float Angle, GLuint Texture)
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(RenderShaderProgramDefault);
		unsigned int ScreenMatrix = glGetUniformLocation(RenderShaderProgramDefault, "ScreenMatrix");
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

		unsigned int WorldMatrixID = glGetUniformLocation(RenderShaderProgramDefault, "WorldMatrix");

		const float WorldMatrix1[4][4] =
		{
			{Scale,0,0,0 },
			{0,Scale,0,0 },
			{0,0,Scale,0 },
			{PosX,PosY,0,1 },
		};

		//Angle *= M_PI;
		const float WorldMatrix2[4][4] =
		{
			{cosf(Angle),sinf(Angle),0,0},
			{-sinf(Angle),cosf(Angle),0,0 },
			{0,0,1,0 },
			{0,0,0,1 },
		};
		float ResultTest[4][4] = {};

		multiply(&WorldMatrix2[0][0], &WorldMatrix1[0][0], &ResultTest[0][0]);

		glUniformMatrix4fv(WorldMatrixID, 1, GL_FALSE, &ResultTest[0][0]);


		glBindVertexArray(RenderVAODefault);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(glGetUniformLocation(RenderShaderProgramDefault, "InTexture"), 0);


		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glDisable(GL_BLEND);
	};

	for (auto Data : RenderList)
	{
		RenderFrame(Data.Scale, Data.x, Data.y, Data.Angle, Data.TextureID);
	}

	RenderList.clear();
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
