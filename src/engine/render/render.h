#pragma once

namespace Asura::render
{
    using texture_id = ImTextureID;

	void init_vulkan();
	
	void pre_init();
	void Init();

	void Destroy();
	void Tick(float dt);

    texture_id get_texture(resources::id_t resource_id);
    texture_id load_texture(resources::id_t resource_id);
}
