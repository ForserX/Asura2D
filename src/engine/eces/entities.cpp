#include "pch.h"

using namespace Asura;

registry global_registry = {};
entity_view invalid_entity = {};

Math::FVec2 no_pos = { FLT_MAX, FLT_MAX };

input::on_key_change entities_key_change_event = {};
bool clear_on_next_tick = false;
bool free_on_next_tick = false;

registry&
entities::internal::get_registry()
{
	return global_registry;
}

void shit_detector_tick() 
{
	auto& reg = global_registry.get();
	auto destroy_ent = [&reg](entt::entity ent)
	{
		if (entities::IsValid(ent)) 
		{
			if (const auto phys_comp = reg.try_get<entities::physics_body_component>(ent))
			{
				Physics::schedule_free(phys_comp->body);
			}

			reg.destroy(ent);
		}
	};

	if (free_on_next_tick) {
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size())
		{	
			entt::entity ent = *ent_ptr;
			if (entities::IsValid(ent) && !entities::contains<entities::garbage_flag>(ent))
			{
				entities::add_field<entities::garbage_flag>(ent);
			}

			ent_ptr++;
		}

		free_on_next_tick = false;
	}
	
	const auto view = reg.view<entities::garbage_flag>();
	view.each([&reg, &destroy_ent](entt::entity ent)
		{
		destroy_ent(ent);
	});

	if (clear_on_next_tick) 
	{
		reg.clear();
		clear_on_next_tick = false;
	}
};

void entities::Init()
{
#ifndef ASURA_SHIPPING
	entities_key_change_event = input::subscribe_key_event([](int16_t scan_code, input::key_state state)
	{	
		if (state == input::key_state::press) 
		{
			switch (scan_code) 
			{
				case SDL_SCANCODE_F6: 
				{
					entities::serialize_state("game_state");
					break;
				}
				case SDL_SCANCODE_F8: 
				{
					entities::deserialize_state("game_state");
					break;
				}
			}
		}
	});
#endif

	Scheduler::schedule(Scheduler::garbage_collector, []() 
	{
		internal::process_entities([]() 
		{
			shit_detector_tick();
		}, entities_state::cleaning_up);

		return true;
	});
}                                                           

void entities::Destroy()
{
#ifndef ASURA_SHIPPING
	input::unsubscribe_key_event(entities_key_change_event);
#endif
}

void entities::Tick(float dt)
{

}

void entities::clear()
{
	clear_on_next_tick = true;
}

void entities::free()
{
	free_on_next_tick = true;
}

bool entities::IsValid(entity_view ent)
{
	return !IsNull(ent) && ent.get() != entt::tombstone && global_registry.get().valid(ent.get());
}

bool entities::IsNull(entity_view ent)
{
	return ent.get() == entt::null;
}

const Math::FVec2& entities::get_position(const entity_view& ent)
{
	if (contains<scene_component>(ent)) 
	{
		const auto scene_comp = try_get<scene_component>(ent.get());
		if (scene_comp != nullptr) 
		{
			return scene_comp->Transform.position();
		}
	}

	return no_pos;
}

entity_view entities::GetEntityByBbody(const b2Body* body)
{
	const auto view = get_view<physics_body_component>();

	for (const auto entity : view) 
	{
		const auto phys_comp = try_get<physics_body_component>(entity);
		if (phys_comp != nullptr && phys_comp->body != nullptr && phys_comp->body->get_body() == body) 
		{
			return entity;
		}	
	}

	return {};
}

entity_view entities::Create()
{
	return global_registry.create();
}

void entities::MarkAsGarbage(const entity_view& ent)
{
	const auto& registry = global_registry.get();
	if (!registry.all_of<dont_free_after_reset_flag>(ent.get()) && !registry.all_of<garbage_flag>(ent.get())) {
		add_field<garbage_flag>(ent.get());
	}
}

const entity_view& entities::AddTexture(const entity_view& ent, stl::string_view path)
{
    const resources::id_t texture_resource = resources::load(path);
	const ImTextureID texture_id = render::load_texture(texture_resource);
	game_assert(texture_id != nullptr, "can't load texture", return ent);

	add_field<draw_texture_component>(ent, texture_resource);
	return ent;
}

const entity_view&
entities::AddPhysBody(
	const entity_view& ent,
	Math::FVec2 vel,
	Math::FVec2 pos,
	Math::FVec2 size,
	Physics::body_type type,
	material::shape shape,
	material::type mat
)
{
	const Physics::body_parameters phys_parameters(0.f, 0.f, vel, pos, size, type, shape, mat);
	Physics::PhysicsBody* body = schedule_creation(phys_parameters);
	
	add_field<physics_body_component>(ent, body);
	if (!contains<scene_component>(ent)) 
	{
		add_field<scene_component>(ent);
	}

	if (contains<draw_color_component>(ent)) 
	{
		erase_field<draw_color_component>(ent);
	}

	return ent;
}

const entity_view& Asura::entities::AddPhysBodyPreset(const entity_view& ent, Math::FVec2 pos, stl::string_view preset)
{
	stl::path preset_file = FileSystem::get_content_dir();
	preset_file.append("bodies").append(preset);

	CfgParser reader;
	reader.load(preset_file);

	stl::tree_string_map parser_data = reader.get_data();

	stl::vector<Physics::PhysicsBody*> new_bodies;

	for (auto [sect, key_val] : parser_data)
	{
		if (sect.find("body") != std::string::npos) 
		{
			Physics::body_parameters phys_parameters = {};
			phys_parameters.pos.x = pos.x + stl::stof(key_val["x_offset"]);
			phys_parameters.pos.y = pos.y + stl::stof(key_val["y_offset"]);
			phys_parameters.angle = 0;
			phys_parameters.angular_vel = 0;
			phys_parameters.vel = { 0, 0 };
			phys_parameters.size.from_string(key_val["size"]);
			phys_parameters.packed_type.shape = 1 + (key_val["shape"] != "box");
			phys_parameters.packed_type.type = 1 + (key_val["type"] == "dynamic");
			phys_parameters.packed_type.mat = 1;

			auto body = new_bodies.emplace_back(schedule_creation(phys_parameters));

			const entity_view& ent_body = Create();

			add_field<physics_body_component>(ent_body, body);
			add_field<scene_component>(ent_body);
			add_field<entities::drawable_flag>(ent_body);
		}
		else if (sect.find("joint") != std::string::npos)
		{
			Physics::joint_data jdata =
			{
				new_bodies[stl::stoull(key_val["first"])] , new_bodies[stl::stoull(key_val["second"])],
				(float)stl::stof(key_val["lower"]), (float)stl::stof(key_val["upper"]),
				key_val["type"] == "revolute" ? Physics::joint_type::revolute : Physics::joint_type::base,
				key_val["limit"] == "true"
			};

			auto joint = schedule_creation(std::move(jdata));

			const entity_view& ent_body = Create();

			add_field<physics_joint_component>(ent_body, joint);
			add_field<scene_component>(ent_body);
			add_field<entities::drawable_flag>(ent_body);
		}
	}

	new_bodies.clear();

	return ent;
}

const entity_view& entities::AddSceneComponent(const entity_view& ent)
{
	add_field<scene_component>(ent);
	add_field<draw_color_component>(ent);

	add_field<entities::drawable_flag>(ent);

	return ent;
}
