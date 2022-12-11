#pragma once
#include "asura.h"

namespace ingame
{
	
class movement_system final : public Asura::system
{
public:
	void Init() override;
	void Tick(float dt) override;
	void Reset() override;
};
	
}