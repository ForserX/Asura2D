#pragma once

namespace Asura::GamePlay::Holder::player
{
	void Init();
	void Tick();
	void Destroy();

	void Attach(EntityBase entity);
	void Detach();
}