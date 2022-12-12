#include "pch.h"

using namespace Asura;

namespace Asura::scene 
{
	CfgParser section_parser;
}

void scene::Init()
{
}

void scene::Destroy()
{

}

void scene::Tick(float dt)
{

}

void scene::close_scene()
{
	auto scene_entities = Entities::get_view<Entities::scene_component>();
	for (auto ent : scene_entities) {
		Entities::MarkAsGarbage(ent);
	}
}

void scene::import_scene(std::string_view scene_name)
{
	std::filesystem::path scene_path = FileSystem::ContentDir();
	scene_path.append("scene");
	scene_path.append(scene_name);

	game_assert(std::filesystem::exists(scene_path), "Can't find scene", return);
	section_parser.load(scene_path);

	//Entities::string_deserialize(section_parser.get_data());
}

void scene::export_scene(std::string_view scene_name)
{
	std::filesystem::path scene_path = FileSystem::ContentDir();
	scene_path.append("scene");
	scene_path.append(scene_name);

	stl::tree_string_map scene_map;
	//Entities::string_serialize(scene_map);

	//section_parser = std::move(CfgParser(std::move(scene_map), section_parser.get_count()));
	//section_parser.save(scene_path);
}
