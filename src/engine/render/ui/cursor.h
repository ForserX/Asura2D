#pragma once

namespace Asura::UI::CursorManager
{
	size_t Register(FileSystem::Path TextureFile);
	void SetActive(size_t ID);
	
	void Tick();
}