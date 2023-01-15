#pragma once

namespace Asura::Render::Shaders
{
	class ShaderProgram
	{
		GLuint RenderShaderProgramDefault;

		GLuint VertexShader;
		GLuint PixelShader;

		stl::string PixelShaderName;
		stl::string VertexShaderName;

	public:
		ShaderProgram(stl::string_view Pixel, stl::string_view Vertex) noexcept;
		virtual ~ShaderProgram();

		bool Build();
		void FreeBuildData();

		GLuint Use();

		void Link();
	};
}