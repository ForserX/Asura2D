#include "pch.h"

using namespace Asura::Systems;
using namespace Asura::Entities;
using namespace Asura::GamePlay;

bool physical_debug_draw = false;

void draw_system::Init()
{
	color_map.resize(4096);

	for (auto& elem : color_map) 
	{
		std::uniform_int_distribution color_dist(55, 255);
		const int red_color = color_dist(r_engine);
		const int green_color = color_dist(r_engine);
		const int blue_color = color_dist(r_engine);
		elem = ImColor(red_color, green_color, blue_color);
	}
}

void draw_system::Reset()
{
}

void draw_system::Tick(float dt)
{
	if (show_console)
		return;

	OPTICK_EVENT("engine draw system Destroy");

	Entities::AccessView([this]()
	{
		const int64_t width = window_width;
		const int64_t height = window_height;
		const auto draw_view = Entities::GetView<drawable_flag>();
		const auto background_view = Entities::GetView<background_flag>();

		{
			OPTICK_EVENT("engine background objects draw");
			background_view.each([this, width, height](entt::entity entity)
			{
				OPTICK_EVENT("background draw");
				if (!Entities::IsValid(entity))
				{
					return;
				}

				game_assert(!Entities::Contains<drawable_flag>(entity), "background entity can't contain draw flag!", return);

				if (Entities::Contains<draw_color_component>(entity))
				{
					const auto draw_color_comp = Entities::TryGet<draw_color_component>(entity);
					if (draw_color_comp != nullptr) 
					{
						Graphics::DrawRect(draw_color_comp->color, Math::FRect(0, 0, width, height));
						return;
					}
				}

				if (Entities::Contains<draw_texture_component>(entity))
				{
					const auto draw_texture_comp = Entities::TryGet<draw_texture_component>(entity);
					if (draw_texture_comp != nullptr) 
					{
						Graphics::DrawBackground(draw_texture_comp->texture_resource, draw_texture_comp->use_parallax);
						return;
					}
				}
			});
		}

		{
			OPTICK_EVENT("engine objects draw");
			draw_view.each([this](entt::entity entity) 
			{
				OPTICK_EVENT("object draw");
				if (!Entities::IsValid(entity))
				{
					return;
				}

				if (const auto phys_comp = Entities::TryGet<physics_body_component>(entity)) 
				{
					const auto PhysBody = phys_comp->body;

					if (PhysBody->IsDestroyed() || !PhysBody->is_enabled())
					{
						return;
					}

					if (phys_comp->IgnoreTest && !Camera::CanSee(PhysBody->get_position()))
					{
						return;
					}

					const auto phys_body_id = reinterpret_cast<ptrdiff_t>(PhysBody);
					switch (static_cast<Physics::Material::shape>(PhysBody->get_parameters().packed_type.shape))
					{
						case Physics::Material::shape::circle:
						{
							auto TextureEntt = Entities::TryGet<draw_texture_component>(entity);

							if (TextureEntt)
							{
								Graphics::DrawTextureObject(PhysBody, TextureEntt->texture_resource);
							}
							else
							{
								Graphics::DrawPhysObjectCircle(PhysBody->get_body(), color_map[phys_body_id % 4096]);
							}
							break;
						}
						default:
						{
							Graphics::DrawPhysObject(PhysBody->get_body(), color_map[phys_body_id % 4096]);
							break;
						}
					}

					return;
				}
				else if (const auto phys_comp = Entities::TryGet<physics_joint_component>(entity)) 
				{
					const auto physical_body = phys_comp->joint;
					if (!physical_body) 
					{
						return;
					}

					const b2Transform& xf1 = physical_body->Get()->GetBodyA()->GetTransform();
					const b2Transform& xf2 = physical_body->Get()->GetBodyB()->GetTransform();

					Math::FVec2 p1 = Camera::World2Screen(xf1.p);
					Math::FVec2 p2 = Camera::World2Screen(xf2.p);

					if (p1.x == p2.x)
					{
						p1.x -= 1;
						p2.x += 1;
					}

					const auto phys_body_id = reinterpret_cast<ptrdiff_t>(physical_body);
					Graphics::DrawRect(color_map[phys_body_id % 4096], { p1, p2 });

					return;
				}
				else if (Entities::ContainsAny<draw_color_component, draw_gradient_component, draw_texture_component>(entity))
				{
					if (const auto scene_comp = Entities::TryGet<scene_component>(entity))
					{
						if (const auto texture_comp = Entities::TryGet<draw_texture_component>(entity))
						{
							if (Camera::CanSee(scene_comp->Transform.position()))
							{
								const auto half_size = Math::FVec2(scene_comp->size.x / 2.f, scene_comp->size.y / 2.f);
								const auto begin_pos = scene_comp->Transform.position() - half_size;
								const auto end_pos = scene_comp->Transform.position() + half_size;
								Graphics::DrawTextureRect(texture_comp->texture_resource, { begin_pos, end_pos });
							}
						}
						else if (const auto color_comp = Entities::TryGet<draw_color_component>(entity))
						{
							if (Camera::CanSee(scene_comp->Transform.position()))
							{
								const auto entt_id = reinterpret_cast<ptrdiff_t>(color_comp);
								const auto half_size = Math::FVec2(scene_comp->size.x / 2.f, scene_comp->size.y / 2.f);
								const auto begin_pos = scene_comp->Transform.position() - half_size;
								const auto end_pos = scene_comp->Transform.position() + half_size;
								Graphics::DrawRect(color_map[entt_id % 4096], { begin_pos, end_pos });
							}
						}
					}
				}
			});
		}
	});

	if (physical_debug_draw) 
	{
		Physics::GetWorld().GetWorld().DebugDraw();
	}
}