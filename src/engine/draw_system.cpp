﻿#include "pch.h"

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
draw_system::tick(float dt)
{
	OPTICK_EVENT("engine draw system tick");

	entities::access([this]() {
		const auto draw_view = entities::get_view<drawable_flag>();
		const auto background_view = entities::get_view<background_flag>();

		background_view.each([this](entt::entity entity) {
			if (!entities::is_valid(entity)) {
				return;
			}

			ark_assert(!entities::contains<drawable_flag>(entity), "background entity can't contain draw flag!", return)

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
					graphics::draw_background(draw_texture_comp->texture);
					return;
				}
			}
		});

		draw_view.each([this](entt::entity entity) {
			// First of all, try to draw physics bodies
			if (entities::is_valid(entity)) {
				if (entities::contains<physics_body_component>(entity)) {
					const auto phys_body = entities::try_get<physics_body_component>(entity);
					if (phys_body == nullptr) {
						return;
					}

					ark_assert(phys_body->body != nullptr, "phys body can't be null!", return)

						const auto physical_body = phys_body->body;
					if (!physical_body->is_created()) {
						return;
					}

					// Object is not created yet, skip it until it was created
					if (physical_body->get_body() == nullptr || !physical_body->get_body()->IsEnabled()) {
						return;
					}

					// If we don't have any of draw components - try to draw physics body with debug view
					if (!entities::contains_any<draw_color_component, draw_gradient_component, draw_texture_component>(entity)) {
						const auto phys_body_id = reinterpret_cast<ptrdiff_t>(physical_body);
						if (!color_map.contains(phys_body_id)) {
							std::uniform_int_distribution color_dist(55, 255);
							const int red_color = color_dist(r_engine);
							const int green_color = color_dist(r_engine);
							const int blue_color = color_dist(r_engine);
							color_map.insert(std::make_pair(phys_body_id, ImColor(red_color, green_color, blue_color)));
						}

						if (physical_body->get_body()->GetFixtureList()->GetType() == b2Shape::Type::e_circle) {
							graphics::draw_physical_circle_object(physical_body->get_body(), color_map[phys_body_id]);
						}
						else {
							graphics::draw_physical_object(physical_body->get_body(), color_map[phys_body_id]);
						}

						return;
					}
				}
			}
			});

		if (physical_debug_draw) {
			physics::get_world().DebugDraw();
		}
	});
}
