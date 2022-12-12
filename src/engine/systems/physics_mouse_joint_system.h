#pragma once

namespace Asura::Systems
{
	class physics_mouse_joint_system final : public system
	{
	public:
		void Init() override;
		void Reset() override;
		void Tick(float dt) override;
	};
}
