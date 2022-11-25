#pragma once

namespace ark::gameplay
{
	enum class holder_mode
	{
		free,
		player
	};

	extern holder_mode holder_type;
}

#include "holder_free.h"
#include "holder_player.h"