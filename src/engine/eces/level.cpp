#include "pch.h"

using namespace Asura;
FileSystem::Path path_level = {};
stl::vector<EntityView> ent_list = {};

namespace Asura::Level::internal
{
    CfgParser level_data = {};

	void load(const FileSystem::Path path)
	{
		level_data.Load(path);

		float level_x = 0;
		float level_y = 0;
		float level_w = 0;
		float level_h = 0;

		for (const auto &[section, kv] : level_data.Data()) 
		{
			if (section == "background") 
			{
				auto Entt = Entities::CreateTexture(level_data.Get<stl::string_view>(section, "path"), level_data.Get<bool>(section, "parallax"));
				Entities::AddField<Entities::background_flag>(Entt);
                Entities::AddField<Entities::scene_component>(Entt);

				ent_list.push_back(std::move(Entt));
				continue;
			}
			
			const bool is_drawable = level_data.Get<bool>(section, "drawable");
			level_x = level_data.Get<float>(section, "x");
			level_y = level_data.Get<float>(section, "y");
			level_w = level_data.Get<float>(section, "w");
			level_h = level_data.Get<float>(section, "h");
					
			const auto body_type = level_data.Get<Physics::body_type>(section, "type");
			const auto body_shape = level_data.Get<Physics::Material::shape>(section, "shape");
			const auto material_type = level_data.Get<Physics::Material::type>(section, "material");
			
			level_w /= 2;
			level_h /= 2;

			level_x = std::max(level_w, level_x) - std::min(level_w, level_x);
			level_y = std::max(level_h, level_y) - std::min(level_h, level_y);

			Physics::body_parameters CurrentBody
			(
				0, 0, {}, // Velocity
				{ level_x, level_y }, { level_w, level_h }, // xy hw
				body_type, body_shape, material_type // ph info
			);

			auto FindResult = section.find("ground_", section.length());

			auto ent = ent_list.emplace_back(Entities::CreatePhysBody(CurrentBody, FindResult != stl::npos));

			if (is_drawable) 
			{
				Entities::AddField<Entities::drawable_flag>(ent);
			}
		}
	};
};

void Asura::Level::Init()
{
	path_level = FileSystem::ContentDir();
	path_level.append("level");

	game_assert(std::filesystem::exists(path_level), "Level path not found! Please, reinstall game!", return);

	for (auto& dir_iter : std::filesystem::directory_iterator{ path_level })
	{
		internal::load(dir_iter);
	}
}

void Asura::Level::Tick(float dt)
{
}

void Asura::Level::Destroy()
{
}