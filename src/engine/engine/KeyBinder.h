#pragma once
#include "pch.h"

namespace Asura::Input
{
	enum class eActions :int
	{
		DoNothing = 0,
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
		EditorSwitchMode
	};
	
	// Есть action, scan_code, key_state.
	// Есть у нас для scan_code и key_state назначен экшн, то мы его выполняем.
	
	
	static std::map<std::pair<int16_t, Asura::Input::key_state>, Asura::Input::eActions> ActionMap;

	void BindNewAction(int16_t scan_code, Asura::Input::key_state state, Asura::Input::eActions action)
	{
		ActionMap[std::make_pair(scan_code, state)] = action;
	}

	Asura::Input::eActions GetActionFromPair(int16_t scan_code, Asura::Input::key_state state)
	{
		return ActionMap[std::make_pair(scan_code, state)];
	}

	//void Callback(Asura::Input::eActions eAction);
		
	







}
