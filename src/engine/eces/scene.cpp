#include "pch.h"

using namespace ark;

namespace ark::scene 
{
	config_parser section_parser;
}

void 
scene::init()
{
}

void
scene::destroy()
{

}

void 
scene::tick(float dt)
{

}

void
scene::close_scene()
{
	auto scene_entities = entities::get_view<entities::scene_component>();
	for (auto ent : scene_entities) {
		entities::mark_as_garbage(ent);
	}
}

void 
scene::import_scene(std::string_view scene_name)
{
	std::filesystem::path scene_path = filesystem::get_content_dir();
	scene_path.append("scene");
	scene_path.append(scene_name);

	ark_assert(std::filesystem::exists(scene_path), "Can't find scene", return);
	section_parser.load(scene_path);

	entities::string_deserialize(section_parser.get_data());
}

void
scene::export_scene(std::string_view scene_name)
{
	std::filesystem::path scene_path = filesystem::get_content_dir();
	scene_path.append("scene");
	scene_path.append(scene_name);

	stl::tree_string_map scene_map;
	entities::string_serialize(scene_map);

	//section_parser = std::move(config_parser(std::move(scene_map), section_parser.get_count()));
	//section_parser.save(scene_path);
}
