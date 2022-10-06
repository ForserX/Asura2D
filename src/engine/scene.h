#pragma once

namespace ark::scene
{
	void init();
	void destroy();
	void tick(float dt);

	void close_scene();
	void import_scene(std::string_view scene_name);
	void export_scene(std::string_view scene_name);
}