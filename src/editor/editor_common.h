#pragma once

namespace asura::editor
{
	void init();
	void destroy();

	namespace object
	{
		void create_fake();
		void update_fake();
		void make_phys();
	}

	namespace ui
	{
		void tick();
	}
};