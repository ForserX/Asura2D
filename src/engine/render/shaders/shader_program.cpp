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

	GLint SuccessBuild;
	GLchar InfoLog[512];
	std::ostringstream StrBuff;

	if (VertexShaderName.length() > 6)
	{
		Path.append("shaders").append(VertexShaderName);

		std::ifstream SFile;
		SFile.open(Path);
		StrBuff << SFile.rdbuf();
		SFile.close();

		GLchar* StrVert = strdup(StrBuff.str().c_str());

		glShaderSource(VertexShader, 1, &StrVert, NULL);
		glCompileShader(VertexShader);

		free(StrVert);

		// Check for compile time errors
		glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &SuccessBuild);
		if (!SuccessBuild)
		{
			glGetShaderInfoLog(VertexShader, 512, NULL, InfoLog);
			game_assert(SuccessBuild, InfoLog, return false);
		}
		glAttachShader(RenderShaderProgramDefault, PixelShader);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// Fragment shader
	if (PixelShaderName.length() > 6)
	{
		Path = FileSystem::ContentDir();
		Path.append("shaders").append(PixelShaderName);
		StrBuff.str("");
		StrBuff.clear();

		std::ifstream VFile;
		VFile.open(Path);
		StrBuff << VFile.rdbuf();
		VFile.close();
		GLchar* StrPix = strdup(StrBuff.str().c_str());

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
	}

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
