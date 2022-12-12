﻿#pragma once

namespace Asura::Audio
{
	interface Device
	{
	public:
		virtual void Load(ResourcesManager::id_t File) = 0;
		virtual void Tick() = 0;
	};

	enum class DeviceMode
	{
		eOpenAL,
		eXAudio2
	};

	void Init();
	void Tick();
	void Destroy();

	void Start(stl::string_view File);
	void Reset(DeviceMode Mode = DeviceMode::eOpenAL);
}