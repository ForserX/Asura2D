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

		float LevelX = 0;
		float LevelY = 0;
		float LevelH = 0;
		float LevelW = 0;

		for (const auto &[Section, kv] : level_data.Data()) 
		{
			if (Section == "background")
			{
				auto Entt = Entities::CreateTexture(level_data.Get<stl::string_view>(Section, "path"), level_data.Get<bool>(Section, "parallax"));
				Entities::AddField<Entities::background_flag>(Entt);
                Entities::AddField<Entities::scene_component>(Entt);

				ent_list.push_back(std::move(Entt));
				continue;
			}
			
			const bool IsDrawable = level_data.Get<bool>(Section, "drawable");
			LevelX = level_data.Get<float>(Section, "x");
			LevelY = level_data.Get<float>(Section, "y");
			LevelW = level_data.Get<float>(Section, "w");
			LevelH = level_data.Get<float>(Section, "h");
					
			const auto PhType = level_data.Get<Physics::BodyType>(Section, "type");
			const auto PhShape = level_data.Get<Physics::Material::shape>(Section, "shape");
			const auto PhMaterial = level_data.Get<Physics::Material::type>(Section, "material");

			const auto Texture = level_data.Get<stl::string>(Section, "texture");
			
			LevelW /= 2;
			LevelH /= 2;

			LevelX = std::max(LevelW, LevelX) - std::min(LevelW, LevelX);
			LevelY = std::max(LevelH, LevelY) - std::min(LevelH, LevelY);

			Physics::body_parameters CurrentBody
			(
				0, 0, {}, // Velocity
				{ LevelX, LevelY }, { LevelW, LevelH }, // xy hw
				PhType, PhShape, PhMaterial // ph info
			);

			auto FindResult = Section.find("ground_", Section.length());

			auto TempEntt = Entities::CreatePhysBody(CurrentBody, FindResult != stl::npos);
			Entities::AddTexture(TempEntt, Texture, false);

			if (IsDrawable)
			{
				Entities::AddField<Entities::drawable_flag>(TempEntt);
			}

			ent_list.push_back(std::move(TempEntt));
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