#pragma once

namespace ark::render
{
	void init_vulkan();
	
	void pre_init();
	void init();

	void destroy();
	void tick(float dt);

	ImTextureID* load_texture(std::string_view path);
}
