#include "pch.h"

using namespace Asura;

Registry global_registry = {};
EntityView invalid_entity = {};

Math::FVec2 no_pos = { FLT_MAX, FLT_MAX };

input::on_key_change entities_key_change_event = {};
bool clear_on_next_tick = false;
bool free_on_next_tick = false;

Registry& Entities::internal::get_registry()
{
	return global_registry;
}

void shit_detector_tick() 
{
	auto& reg = global_registry.Get();
	auto destroy_ent = [&reg](entt::entity ent)
	{
		if (Entities::IsValid(ent)) 
		{
			if (const auto phys_comp = reg.try_get<Entities::physics_body_component>(ent))
			{
				Physics::SafeFree(phys_comp->body);
			}

			reg.destroy(ent);
		}
	};

	if (free_on_next_tick)
	{
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size())
		{	
			entt::entity ent = *ent_ptr;
			if (Entities::IsValid(ent) && !Entities::contains<Entities::garbage_flag>(ent))
			{
				Entities::add_field<Entities::garbage_flag>(ent);
			}

			ent_ptr++;
		}

		free_on_next_tick = false;
	}
	
	const auto view = reg.view<Entities::garbage_flag>();
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

void Entities::Init()
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
					Entities::serialize_state("game_state");
					break;
				}
				case SDL_SCANCODE_F8: 
				{
					Entities::deserialize_state("game_state");
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

void Entities::Destroy()
{
#ifndef ASURA_SHIPPING
	input::unsubscribe_key_event(entities_key_change_event);
#endif
}

void Entities::Tick(float dt)
{

}

void Entities::clear()
{
	clear_on_next_tick = true;
}

void Entities::free()
{
	free_on_next_tick = true;
}

bool Entities::IsValid(EntityView ent)
{
	return !IsNull(ent) && ent.Get() != entt::tombstone && global_registry.Get().valid(ent.Get());
}

bool Entities::IsNull(EntityView ent)
{
	return ent.Get() == entt::null;
}

const Math::FVec2& Entities::get_position(const EntityView& ent)
{
	if (contains<scene_component>(ent)) 
	{
		const auto scene_comp = try_get<scene_component>(ent.Get());
		if (scene_comp != nullptr) 
		{
			return scene_comp->Transform.position();
		}
	}

	return no_pos;
}

EntityView Entities::GetEntityByBbody(const b2Body* body)
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

EntityView Entities::Create()
{
	return global_registry.Create();
}

void Entities::MarkAsGarbage(const EntityView& ent)
{
	const auto& registry = global_registry.Get();
	if (!registry.all_of<dont_free_after_reset_flag>(ent.Get()) && !registry.all_of<garbage_flag>(ent.Get())) 
	{
		add_field<garbage_flag>(ent.Get());
	}
}

const EntityView& Entities::AddTexture(const EntityView& ent, stl::string_view path)
{
    const ResourcesManager::id_t texture_resource = ResourcesManager::Load(path);
	const ImTextureID texture_id = Render::LoadTexture(texture_resource);
	game_assert(texture_id != nullptr, "can't load texture", return ent);

	add_field<draw_texture_component>(ent, texture_resource);
	return ent;
}

const EntityView&
Entities::AddPhysBody(
	const EntityView& ent,
	Math::FVec2 vel,
	Math::FVec2 pos,
	Math::FVec2 size,
	Physics::body_type type,
	Physics::Material::shape shape,
	Physics::Material::type mat
)
{
	const Physics::body_parameters phys_parameters(0.f, 0.f, vel, pos, size, type, shape, mat);
	Physics::PhysicsBody* body = SafeCreation(phys_parameters);
	
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

const EntityView& Asura::Entities::AddPhysBodyPreset(const EntityView& ent, Math::FVec2 pos, stl::string_view preset)
{
	stl::path preset_file = FileSystem::ContentDir();
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

			auto body = new_bodies.emplace_back(SafeCreation(phys_parameters));

			const EntityView& ent_body = Create();

			add_field<physics_body_component>(ent_body, body);
			add_field<scene_component>(ent_body);
			add_field<Entities::drawable_flag>(ent_body);
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

			auto joint = SafeCreation(std::move(jdata));

			const EntityView& ent_body = Create();

			add_field<physics_joint_component>(ent_body, joint);
			add_field<scene_component>(ent_body);
			add_field<Entities::drawable_flag>(ent_body);
		}
	}

	new_bodies.clear();

	return ent;
}

const EntityView& Entities::AddSceneComponent(const EntityView& ent)
{
	add_field<scene_component>(ent);
	add_field<draw_color_component>(ent);

	add_field<Entities::drawable_flag>(ent);

	return ent;
}
