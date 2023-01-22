#pragma once

namespace Asura::Systems
{
	class PhysicsSystem final : public ISystem
	{
	private:


	public:
		void Init() override;
		void Reset() override;
		void Tick(float dt) override;
	};
}
