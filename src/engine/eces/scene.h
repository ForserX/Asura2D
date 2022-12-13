#pragma once

namespace Asura::Scene
{
	void Init();
	void Destroy();
	void Tick(float dt);

	void Close();
	void Import(std::string_view scene_name);
	void Export(std::string_view scene_name);
}