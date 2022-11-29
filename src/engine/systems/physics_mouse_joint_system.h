#pragma once

namespace asura::systems
{
	class physics_mouse_joint_system final : public system
	{
	public:
		void init() override;
		void reset() override;
		void tick(float dt) override;
	};
}
