#include "pch.h"
#include "tex_rotation.h"

using namespace Asura;
using namespace Asura::Render::Shaders;

TextureRotation::TextureRotation()
{
	TextureShader = new Shaders::ShaderProgram("image_rotation_ps.glsl", "image_rotation_vs.glsl");
}

TextureRotation::~TextureRotation()
{
	delete(TextureShader);
}

void TextureRotation::Build()
{
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

void TextureRotation::Render(const RenderData& Data)
{
	RenderInstance(Data.ScaleX, Data.ScaleY, Data.x, Data.y, Data.Angle, Data.TextureID);
}

void TextureRotation::RenderInstance(float ScaleX, float ScaleY, float PosX, float PosY, float Angle, GLuint Texture)
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
}