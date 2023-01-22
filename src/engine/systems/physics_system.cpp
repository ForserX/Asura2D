#include "pch.h"

using namespace Asura::Systems;

constexpr float y_destroy_coord = -100.f;

void PhysicsSystem::Init()
{
}

void PhysicsSystem::Reset()
{
}

void PhysicsSystem::Tick(float dt)
{
	OPTICK_EVENT("engine Physics system Destroy");

	Entities::AccessView([]() 
	{
		const auto view = Entities::GetView<Entities::physics_body_component, Entities::scene_component>();
		view.each([](const entt::entity ent, Entities::physics_body_component& phys_comp, Entities::scene_component& scene_comp) 
		{
			if (Entities::IsValid(ent)) 
			{
				if (scene_comp.Transform.position().y < y_destroy_coord) 
				{
					Entities::MarkAsGarbage(ent);
					return;
				}
			}
		});
	});
}
