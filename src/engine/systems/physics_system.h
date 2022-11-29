#pragma once

namespace asura::systems
{
	class physics_system final : public system
	{
	private:


	public:
		void init() override;
		void reset() override;
		void tick(float dt) override;
	};
}
