#pragma once

namespace Asura::scene
{
	void Init();
	void Destroy();
	void Tick(float dt);

	void close_scene();
	void import_scene(std::string_view scene_name);
	void export_scene(std::string_view scene_name);
}