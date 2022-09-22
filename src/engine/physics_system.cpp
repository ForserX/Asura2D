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
	const auto &view = registry.view<entities::physics_body_component>();
	for (auto& entity : view) {
		const auto phys_body = registry.get<entities::physics_body_component>(entity);
		if (phys_body.body != nullptr) {
			const b2Vec2& pos =	phys_body.body->get_position();
			if (pos.y < y_destroy_coord) {
				entities::schedule_to_destroy_entity(entity);
				continue;
			}
		}
	}
}
