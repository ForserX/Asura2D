#include "pch.h"

using namespace Asura::systems;

constexpr float y_destroy_coord = -100.f;

void physics_system::Init()
{
}

void physics_system::Reset()
{
}

void physics_system::Tick(float dt)
{
	OPTICK_EVENT("engine Physics system Destroy");

	entities::access_view([]() 
	{
		const auto view = entities::get_view<entities::physics_body_component, entities::scene_component>();
		view.each([](const entt::entity ent, entities::physics_body_component& phys_comp, entities::scene_component& scene_comp) 
		{
			if (entities::IsValid(ent)) 
			{
				if (scene_comp.Transform.position().y < y_destroy_coord) 
				{
					entities::MarkAsGarbage(ent);
					return;
				}
			}
		});
	});
}
