#include "pch.h"

using namespace Asura;

namespace Asura::Scene 
{
	CfgParser Parser;
}

void Scene::Init()
{
	Parser = {};
}

void Scene::Destroy()
{

}

void Scene::Tick(float dt)
{

}

void Scene::Close()
{
	auto scene_entities = Entities::GetView<Entities::scene_component>();

	for (auto ent : scene_entities) 
	{
		Entities::MarkAsGarbage(ent);
	}
}

void Scene::Import(std::string_view scene_name)
{
	FileSystem::Path scene_path = FileSystem::ContentDir();
	scene_path.append("Scene");
	scene_path.append(scene_name);

	game_assert(std::filesystem::exists(scene_path), "Can't find Scene", return);
	Parser.Load(scene_path);

	Entities::string_deserialize(Parser.Data());
}

void Scene::Export(std::string_view scene_name)
{
	FileSystem::Path scene_path = FileSystem::ContentDir();
	scene_path.append("Scene");
	scene_path.append(scene_name);

	stl::tree_string_map scene_map;
	Entities::string_serialize(scene_map);

	Parser.Swap(scene_map);
	Parser.Save(scene_path);
}
