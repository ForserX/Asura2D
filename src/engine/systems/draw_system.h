#pragma once

namespace Asura::systems
{
	class draw_system final : public system
	{
	private:
		std::random_device r_device;
		std::default_random_engine r_engine;

		stl::vector<ImColor> color_map;
		
	public:
		draw_system() : r_engine(r_device()) {}
		
		void Init() override;
		void Reset() override;
		void Tick(float dt) override;
	};
}
