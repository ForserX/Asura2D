#pragma once

namespace Asura::systems
{
	class physics_system final : public system
	{
	private:


	public:
		void Init() override;
		void Reset() override;
		void Tick(float dt) override;
	};
}
