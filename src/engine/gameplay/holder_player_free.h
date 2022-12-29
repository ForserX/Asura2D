#pragma once

namespace Asura::GamePlay::Holder::PlayerFree
{
	void Init();
	void Tick();
	void Destroy();

	void Bind(EntityBase entity);
}