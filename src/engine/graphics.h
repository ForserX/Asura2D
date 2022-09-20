#pragma once

namespace ark::graphics
{
	void pre_init();
	void init();
	void init_vulkan();

	void destroy();
	void tick();

	namespace theme
	{
		enum class stryle
		{
			red,
			dark,
			invalid
		};

		void red();
		void dark();
		void change();
	}
}
