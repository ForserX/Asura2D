#pragma once

namespace Asura::Render::Shaders
{
	class TextureRotation
	{
		ShaderProgram* TextureShader = nullptr;

	private:
		void RenderInstance(float ScaleX, float ScaleY, float PosX, float PosY, float Angle, GLuint Texture);
	public:
		TextureRotation();
		~TextureRotation();

		void Build();
		void Render(const RenderData& Data);
	};
}