#pragma once

namespace Asura::Systems
{
	class RenderSystem final : public ISystem
	{
	private:
		std::random_device r_device;
		std::default_random_engine r_engine;

		stl::vector<ImColor> color_map;
		
	public:
		RenderSystem() : r_engine(r_device()) {}
		
		void Init() override;
		void Reset() override;
		void Tick(float dt) override;
	};
}
