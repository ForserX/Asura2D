#pragma once
#include "arkane.h"

namespace ingame
{
	
class movement_system final : public ark::system
{
public:
	void init() override;
	void tick(ark::registry& reg, float dt) override;
	void reset() override;
};
	
}