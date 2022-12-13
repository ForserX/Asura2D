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
		free,
		player
	};
}

#include "holder_free.h"
#include "holder_player.h"