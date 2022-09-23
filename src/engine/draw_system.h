#pragma once

namespace ark::systems
{
	class draw_system final : public system
	{
	private:
		std::random_device r_device;
		std::default_random_engine r_engine;

		std::unordered_map<ptrdiff_t, ImColor> color_map;
		
	public:
		draw_system() : r_engine(r_device()) {}
		
		void init() override;
		void reset() override;
		void tick(registry& reg, float dt) override;
	};
}
