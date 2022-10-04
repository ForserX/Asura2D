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
physics_system::tick(registry& reg, float dt)
{
	OPTICK_EVENT("engine physics system tick")
	entt::registry& registry = reg.get();
	const auto view = registry.view<entities::physics_body_component>();
	for (const auto entity : view) {
		if (entities::is_valid(entity)) {
			auto phys_body = registry.try_get<entities::physics_body_component>(entity);
			auto scene_comp = registry.try_get<entities::scene_component>(entity);
			if (phys_body != nullptr && phys_body->body != nullptr) {
				const auto& pos = phys_body->body->get_position();
				if (scene_comp != nullptr) {
					scene_comp->position = pos;
				}

				if (pos.y < y_destroy_coord) {
					entities::mark_as_garbage(entity);
					continue;
				}
			}
		}
	}
}
