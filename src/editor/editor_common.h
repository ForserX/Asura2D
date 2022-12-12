#pragma once

namespace Asura::editor
{
	void Init();
	void Destroy();

	namespace object
	{
		void create_fake();
		void update_fake();
		void make_phys();
	}

	namespace UI
	{
		void Destroy();
	}
};