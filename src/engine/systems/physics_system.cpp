#include "pch.h"

using namespace ark::systems;

constexpr float y_destroy_coord = -100.f;

void
physics_system::init()
{
}

void
physics_system::reset()
{
}

void
physics_system::tick(float dt)
{
	OPTICK_EVENT("engine physics system tick");

	entities::access_view([]() {
		const auto view = entities::get_view<entities::physics_body_component, entities::scene_component>();
		view.each([](const entt::entity ent, entities::physics_body_component& phys_comp, entities::scene_component& scene_comp) {
			if (entities::is_valid(ent)) {
				if (scene_comp.transform.position().y < y_destroy_coord) {
					entities::mark_as_garbage(ent);
					return;
				}
			}
		});
	});
}
