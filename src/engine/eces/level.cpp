#include "pch.h"

using namespace Asura;
std::filesystem::path path_level = {};
stl::vector<EntityView> ent_list = {};

namespace Asura::Level::internal
{
    CfgParser level_data = {};

	void load(const std::filesystem::path path)
	{
		level_data.load(path);

		float level_x = 0;
		float level_y = 0;
		float level_w = 0;
		float level_h = 0;

		// сука, это нужно будет переделать
		for (const auto &[section, kv] : level_data.get_data()) 
		{
			if (section == "background") 
			{
				auto background_ent = ent_list.emplace_back(Entities::AddTexture(Entities::Create(), level_data.get<stl::string_view>(section, "path")));
				Entities::add_field<Entities::background_flag>(background_ent);
                Entities::add_field<Entities::scene_component>(background_ent);
				continue;
			}
			
			const bool is_drawable = level_data.get<bool>(section, "drawable");
			level_x = level_data.get<float>(section, "x");
			level_y = level_data.get<float>(section, "y");
			level_w = level_data.get<float>(section, "w");
			level_h = level_data.get<float>(section, "h");
					
			const auto body_type = level_data.get<Physics::body_type>(section, "type");
			const auto body_shape = level_data.get<Physics::Material::shape>(section, "shape");
			const auto material_type = level_data.get<Physics::Material::type>(section, "material");
			
			level_w /= 2;
			level_h /= 2;

			level_x = std::max(level_w, level_x) - std::min(level_w, level_x);
			level_y = std::max(level_h, level_y) - std::min(level_h, level_y);

			auto ent = ent_list.emplace_back(
				Entities::AddPhysBody(
					Entities::Create(),
					{},
					{ level_x, level_y },
					{ level_w, level_h },
					body_type,
					body_shape,
					material_type
				)
			);

			if (is_drawable) 
			{
				Entities::add_field<Entities::drawable_flag>(ent);
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