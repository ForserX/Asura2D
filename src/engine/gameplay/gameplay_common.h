#pragma once

namespace Asura::GamePlay
{
	enum class MoveWays : int8_t
	{
		left,
		right,
		up,
		down
	};

	enum class holder_mode
	{
		Free,
		Player,
		PlayerFree
	};
}

#include "holder_free.h"
#include "holder_player.h"
#include "holder_player_free.h"