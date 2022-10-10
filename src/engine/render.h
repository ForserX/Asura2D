#pragma once

namespace ark::render
{
	void init_vulkan();
	
	void pre_init();
	void init();

	void destroy();
	void tick(float dt);

    ImTextureID get_texture(int32_t resource_id);
	ImTextureID load_texture(int32_t resource_id);
}
