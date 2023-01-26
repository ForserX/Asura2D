#pragma once

namespace Asura::Render::Shaders
{
	class TextureMirror
	{
		ShaderProgram* TextureShader = nullptr;

	private:
		void RenderInstance(float ScaleX, float ScaleY, float PosX, float PosY, float Angle, GLuint Texture);
	public:
		TextureMirror();
		~TextureMirror();

		void Build();
		void Render(const RenderData& Data);
	};
}