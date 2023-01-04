#pragma once
#include "pch.h"

namespace Asura::Input
{
	enum eActions : uint8_t
	{
		Default = 0,

		CameraMoveRight,
		CameraMoveLeft,
		CameraMoveDown,
		CameraMoveUp,

		PersonMoveLeft,
		PersonMoveRight,
		PersonMoveUp,
		PersonMoveDown,

		HolderAttachToCam,
		EditorSwitchMode,

		// For game library. Use first value for your new enum: ReservedValue + 1
		ReservedValue,

		DoNothing = std::numeric_limits<uint8_t>::max()
	};
	
	// Есть action, scan_code, key_state.
	// Есть у нас для scan_code и key_state назначен экшн, то мы его выполняем.
	
	extern std::map<std::pair<int16_t, State>, eActions> ActionMap;

	inline void Bind(int16_t scan_code, State state, eActions action)
	{
		ActionMap[std::make_pair(scan_code, state)] = action;
	}

	inline eActions Get(int16_t scan_code, State state)
	{
		return ActionMap[std::make_pair(scan_code, state)];
	}
}
