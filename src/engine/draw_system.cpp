#include "pch.h"

using namespace ark::systems;
using namespace ark::entities;

bool physical_debug_draw = false;

void
draw_system::init()
{
}

void
draw_system::reset()
{
}

void
draw_system::tick(registry& reg, float dt)
{
	OPTICK_EVENT("engine draw system tick")
	entt::registry& registry = reg.get();
	const auto &view = registry.view<drawable_flag>();
	for (auto& entity : view) {
		const bool can_draw = registry.any_of<physics_body_component, screen_visual_component, visual_component>(entity);
		ark_assert(can_draw, "draw flag was setted, but entity can't be showed because doesn't have any of valiable component", continue);

		// First of all, try to draw physics bodies
		if (registry.all_of<physics_body_component>(entity)) {
			const auto phys_body = registry.try_get<physics_body_component>(entity);
			if (phys_body == nullptr) {
				continue;
			}
			
			ark_assert(phys_body->body != nullptr, "phys body can't be null!", continue);

			const auto physical_body = phys_body->body;
			if (!physical_body->is_created()) {
				continue;
			}
			
			// Object is not created yet, skip it until it was created
			if (physical_body->get_body() == nullptr || !physical_body->get_body()->IsEnabled()) {
				continue;
			}

			// If we don't have any of draw components - try to draw physics body with debug view
			if (!registry.any_of<draw_color_component, draw_gradient_component, draw_texture_component>(entity)) {
				const auto phys_body_id = reinterpret_cast<ptrdiff_t>(physical_body);
				if (!color_map.contains(phys_body_id)) {
					std::uniform_int_distribution color_dist(55, 255);
					const int red_color = color_dist(r_engine);
					const int green_color = color_dist(r_engine);
					const int blue_color = color_dist(r_engine);
					color_map.insert(std::make_pair(phys_body_id, ImColor(red_color, green_color, blue_color)));
				}
			
				if (physical_body->get_body()->GetFixtureList()->GetType() == b2Shape::Type::e_circle) {
					graphics::draw_physical_cricle_object(physical_body->get_body(), color_map[phys_body_id]);
				} else {
					graphics::draw_physical_object(physical_body->get_body(), color_map[phys_body_id]);
				}
			
				continue;
			}
		}
	}

	if (physical_debug_draw) {
		physics::get_world().DebugDraw();
	}
}
