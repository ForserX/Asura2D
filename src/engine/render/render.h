#pragma once

namespace Asura::Render
{
	struct RenderData
	{
		int x;
		int y;
		float Scale;
		float Angle;
		uint32_t TextureID;
	};

    using texture_id = ImTextureID;

	void Init();

	void Destroy();
	void Tick(float dt);

	void Push(RenderData Data);

    texture_id GetTexture(ResourcesManager::id_t resource_id);
    texture_id LoadTexture(ResourcesManager::id_t resource_id);
}
