#include "pch.h"

using namespace Asura;

Registry global_registry = {};
EntityView invalid_entity = {};

Math::FVec2 no_pos = { FLT_MAX, FLT_MAX };

static int64_t EnttInputID = 0;
bool clear_on_next_tick = false;
bool free_on_next_tick = false;

Registry& Entities::internal::GetRegistry()
{
	return global_registry;
}

void shit_detector_tick() 
{
	auto& Reg = global_registry.Get();
	auto destroy_ent = [&Reg](entt::entity ent)
	{
		if (Entities::IsValid(ent)) 
		{
			if (auto phys_comp = Reg.try_get<Entities::physics_body_component>(ent))
			{
				phys_comp->body->SetAsGarbage();
				Physics::SafeFree(phys_comp->body);
			}

			Reg.destroy(ent);
		}
	};

	if (free_on_next_tick)
	{
		const entt::entity* ent_ptr = Reg.data();
		while (ent_ptr != Reg.data() + Reg.size())
		{	
			entt::entity ent = *ent_ptr;
			if (Entities::IsValid(ent) && !Entities::Contains<Entities::garbage_flag>(ent))
			{
				Entities::AddField<Entities::garbage_flag>(ent);
			}

			ent_ptr++;
		}

		free_on_next_tick = false;
	}
	
	const auto view = Reg.view<Entities::garbage_flag>();
	view.each([&Reg, &destroy_ent](entt::entity ent)
	{
		destroy_ent(ent);
	});

	if (clear_on_next_tick) 
	{
		Reg.clear();
		clear_on_next_tick = false;
	}
};

void Entities::Init()
{
#ifndef ASURA_SHIPPING
	EnttInputID = Input::Emplace([](int16_t scan_code, Input::key_state state)
	{	
		if (state == Input::key_state::press) 
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

	Scheduler::Schedule(Scheduler::garbage_collector, []() 
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
	Input::Erase(EnttInputID);
#endif
}

void Entities::Tick(float dt)
{

}

void Entities::Clear()
{
	clear_on_next_tick = true;
}

void Entities::Free()
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

const Math::FVec2& Entities::GetPosition(const EntityView& ent)
{
	if (Contains<scene_component>(ent)) 
	{
		const auto scene_comp = TryGet<scene_component>(ent.Get());
		if (scene_comp != nullptr) 
		{
			return scene_comp->Transform.position();
		}
	}

	return no_pos;
}

EntityBase Entities::GetEntityByBbody(const b2Body* body)
{
	const auto view = GetView<physics_body_component>();

	for (const auto entity : view) 
	{
		const auto phys_comp = TryGet<physics_body_component>(entity);
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
		AddField<garbage_flag>(ent.Get());
	}
}

void Entities::AddTexture(const EntityView& ent, stl::string_view path, bool Parallax)
{
    const ResourcesManager::id_t texture_resource = ResourcesManager::Load(path);
	const ImTextureID texture_id = Render::LoadTexture(texture_resource);
	game_assert(texture_id != nullptr, "can't load texture", return);

	AddField<draw_texture_component>(ent, texture_resource, Parallax);
}

void Entities::AddPhysBody(const EntityView& ent, const Physics::body_parameters& ParamRef)
{
	Physics::PhysicsBody* body = SafeCreation(ParamRef);
	
	AddField<physics_body_component>(ent, body);
	if (!Contains<scene_component>(ent)) 
	{
		AddField<scene_component>(ent);
	}

	if (Contains<draw_color_component>(ent)) 
	{
		EraseField<draw_color_component>(ent);
	}
}

void Asura::Entities::AddPhysBodyPreset(const EntityView& ent, Math::FVec2 pos, stl::string_view preset)
{
	FileSystem::Path preset_file = FileSystem::ContentDir();
	preset_file.append("bodies").append(preset);

	CfgParser reader;
	reader.Load(preset_file);

	stl::tree_string_map parser_data = reader.Data();

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

			AddField<physics_body_component>(ent_body, body);
			AddField<scene_component>(ent_body);
			AddField<Entities::drawable_flag>(ent_body);
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

			AddField<physics_joint_component>(ent_body, joint);
			AddField<scene_component>(ent_body);
			AddField<Entities::drawable_flag>(ent_body);
		}
	}

	new_bodies.clear();
}

void Entities::AddSceneComponent(const EntityView& ent)
{
	AddField<scene_component>(ent);
	AddField<draw_color_component>(ent);

	AddField<Entities::drawable_flag>(ent);
}
