#include "pch.h"

using namespace Asura;
using namespace Asura::Render;

Shaders::ShaderProgram::ShaderProgram(stl::string_view Pixel, stl::string_view Vertex) noexcept
	: PixelShaderName(Pixel.data()), VertexShaderName(Vertex.data())
{
	RenderShaderProgramDefault = glCreateProgram();
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	PixelShader = glCreateShader(GL_FRAGMENT_SHADER);
}

Shaders::ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(RenderShaderProgramDefault);
}

bool Shaders::ShaderProgram::Build()
{
	auto Path = FileSystem::ContentDir();
	Path.append("shaders").append(VertexShaderName);
	std::ostringstream sstr;

	std::ifstream SFile;
	SFile.open(Path);
	sstr << SFile.rdbuf();
	SFile.close();

	GLchar* StrVert = strdup(sstr.str().c_str());

	glShaderSource(VertexShader, 1, &StrVert, NULL);
	glCompileShader(VertexShader);

	free(StrVert);

	// Check for compile time errors
	GLint SuccessBuild;
	GLchar InfoLog[512];
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &SuccessBuild);
	if (!SuccessBuild)
	{
		glGetShaderInfoLog(VertexShader, 512, NULL, InfoLog);
		game_assert(SuccessBuild, InfoLog, return false);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// Fragment shader
	Path = FileSystem::ContentDir();
	Path.append("shaders").append("image_rotation_ps.glsl");
	sstr.str("");
	sstr.clear();

	std::ifstream VFile;
	VFile.open(Path);
	sstr << VFile.rdbuf();
	VFile.close();
	GLchar* StrPix = strdup(sstr.str().c_str());

	glShaderSource(PixelShader, 1, &StrPix, NULL);
	glCompileShader(PixelShader);
	free(StrPix);

	// Check for compile time errors
	glGetShaderiv(PixelShader, GL_COMPILE_STATUS, &SuccessBuild);

	if (!SuccessBuild)
	{
		glGetShaderInfoLog(PixelShader, 512, NULL, InfoLog);
		game_assert(SuccessBuild, InfoLog, return false);
	}

	glAttachShader(RenderShaderProgramDefault, VertexShader);
	glAttachShader(RenderShaderProgramDefault, PixelShader);

	return true;
}

void Shaders::ShaderProgram::FreeBuildData()
{
	glDeleteShader(VertexShader);
	glDeleteShader(PixelShader);
}

GLuint Shaders::ShaderProgram::Use()
{
	glUseProgram(RenderShaderProgramDefault);
	return RenderShaderProgramDefault;
}

void Shaders::ShaderProgram::Link()
{
	GLint SuccessLink;
	GLchar InfoLog[512];

	// Check for linking errors
	glLinkProgram(RenderShaderProgramDefault);
	glGetProgramiv(RenderShaderProgramDefault, GL_LINK_STATUS, &SuccessLink);

	if (!SuccessLink)
	{
		glGetProgramInfoLog(RenderShaderProgramDefault, 512, NULL, InfoLog);
		game_assert(SuccessLink, InfoLog, return);
	}

}
