#pragma once
#include "asura.h"

namespace ingame
{
	
class movement_system final : public asura::system
{
public:
	void init() override;
	void tick(float dt) override;
	void reset() override;
};
	
}