#pragma once

namespace Asura::GamePlay::Holder::Player
{
	void Init();
	void Tick();
	void Destroy();

	void Bind(EntityView entity);
	void Unbind();
}