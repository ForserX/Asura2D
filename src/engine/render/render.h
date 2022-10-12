#pragma once

namespace ark::render
{
    using texture_id = ImTextureID;

	void init_vulkan();
	
	void pre_init();
	void init();

	void destroy();
	void tick(float dt);

    texture_id get_texture(resources::id_type resource_id);
    texture_id load_texture(resources::id_type resource_id);
}
