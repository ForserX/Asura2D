#pragma once

namespace Asura::Render
{
    using texture_id = ImTextureID;

	void init_vulkan();
	
	void InitialSDLDevice();
	void Init();

	void Destroy();
	void Tick(float dt);

    texture_id GetTexture(ResourcesManager::id_t resource_id);
    texture_id LoadTexture(ResourcesManager::id_t resource_id);
}
