#include "pch.h"

using namespace ark::systems;
using namespace ark::entities;

bool physical_debug_draw = false;

void
draw_system::init()
{
	color_map.resize(4096);

	for (auto& elem : color_map) {
		std::uniform_int_distribution color_dist(55, 255);
		const int red_color = color_dist(r_engine);
		const int green_color = color_dist(r_engine);
		const int blue_color = color_dist(r_engine);
		elem = ImColor(red_color, green_color, blue_color);
	}
}

void
draw_system::reset()
{
}

void
draw_system::tick(float dt)
{
	OPTICK_EVENT("engine draw system tick");

	entities::access([this]() {
		const auto draw_view = entities::get_view<drawable_flag>();
		const auto background_view = entities::get_view<background_flag>();

		{
			OPTICK_EVENT("engine background objects draw");
			background_view.each([this](entt::entity entity) {
				OPTICK_EVENT("background draw");
				if (!entities::is_valid(entity)) {
					return;
				}

                ark_assert(!entities::contains<drawable_flag>(entity), "background entity can't contain draw flag!", return);

				const int64_t width = ui::get_cmd_int("window_width");
				const int64_t height = ui::get_cmd_int("window_height");
				if (entities::contains<draw_color_component>(entity)) {
					const auto draw_color_comp = entities::try_get<draw_color_component>(entity);
					if (draw_color_comp != nullptr) {
						graphics::draw_rect(
							draw_color_comp->color,
							{ 0,0 },
							{ static_cast<float>(width), static_cast<float>(height) }
						);

						return;
					}
				}

				if (entities::contains<draw_texture_component>(entity)) {
					const auto draw_texture_comp = entities::try_get<draw_texture_component>(entity);
					if (draw_texture_comp != nullptr) {
						graphics::draw_background(draw_texture_comp->texture_resource);
						return;
					}
				}
			});
		}

		{
			OPTICK_EVENT("engine objects draw");
			draw_view.each([this](entt::entity entity) {
				OPTICK_EVENT("object draw");
				if (!entities::is_valid(entity)) {
					return;
				}

				// First of all, try to draw physics bodies
				if (const auto phys_comp = entities::try_get<physics_body_component>(entity)) {
					const auto physical_body = phys_comp->body;
					if (!physical_body->is_enabled()) {
						return;
					}

					// If we don't have any of draw components - try to draw physics body with debug view
					if (!entities::contains_any<draw_color_component, draw_gradient_component, draw_texture_component>(entity)) {
						const auto phys_body_id = reinterpret_cast<ptrdiff_t>(physical_body);
						switch (static_cast<material::shape>(physical_body->get_parameters().packed_type.shape)) {
						case material::shape::circle:
							graphics::draw_physical_circle_object(physical_body->get_body(), color_map[phys_body_id % 4096]);
							break;
						default:
							graphics::draw_physical_object(physical_body->get_body(), color_map[phys_body_id % 4096]);
							break;
						}

						return;
					}
				}
			});
		}

		if (physical_debug_draw) {
			physics::get_world().DebugDraw();
		}
	});
}
