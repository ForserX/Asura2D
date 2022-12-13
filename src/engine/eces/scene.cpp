#include "pch.h"

using namespace Asura;

namespace Asura::Scene 
{
	CfgParser section_parser;
}

void Scene::Init()
{
	section_parser = {};
}

void Scene::Destroy()
{

}

void Scene::Tick(float dt)
{

}

void Scene::Close()
{
	auto scene_entities = Entities::get_view<Entities::scene_component>();

	for (auto ent : scene_entities) 
	{
		Entities::MarkAsGarbage(ent);
	}
}

void Scene::Import(std::string_view scene_name)
{
	std::filesystem::path scene_path = FileSystem::ContentDir();
	scene_path.append("Scene");
	scene_path.append(scene_name);

	game_assert(std::filesystem::exists(scene_path), "Can't find Scene", return);
	section_parser.load(scene_path);

	Entities::string_deserialize(section_parser.get_data());
}

void Scene::Export(std::string_view scene_name)
{
	std::filesystem::path scene_path = FileSystem::ContentDir();
	scene_path.append("Scene");
	scene_path.append(scene_name);

	stl::tree_string_map scene_map;
	Entities::string_serialize(scene_map);

	section_parser.swap(scene_map);
	section_parser.save(scene_path);
}
