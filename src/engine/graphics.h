#pragma once

namespace ark::graphics
{
	void pre_init();
	void init();
	void init_vulkan();

	void destroy();
	void tick();
}
