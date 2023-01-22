#pragma once

namespace Asura
{
	class ISystem : entt::process<ISystem, float>
	{
	public:
		ISystem(ISystem&&) = default;
		ISystem() = default;

		virtual ~ISystem() = default;

	public:
		virtual void Init() = 0;
		virtual void Reset() = 0;
		virtual void Tick(float dt) = 0;

	};

	namespace Systems
	{
		enum class UpdateType
		{
			befor,
			update,
			after,
			render,
			physics
		};

		void Unsubscribe(ISystem* InObj, UpdateType Type);
		void Subscribe(ISystem* InObj, UpdateType Type);

		void PreInit();
		void Init();
		void Destroy();

		void BeforTick(float dt);
		void Tick(float dt);
		void AfterTick(float dt);
		void RenderTick(float dt);
		void PhysTick(float dt);
	}
}