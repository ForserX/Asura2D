﻿#include "pch.h"
#include "module_OpenAL.h"

#ifdef OS_WINDOWS
#include "module_XAudio2.h"
#endif

using namespace Asura;
using Asura::Audio::Device;

float Volume = 1.f;

class DeviceDummy : public Device
{
public:
	DeviceDummy() = default;
	~DeviceDummy() = default;

	virtual void Tick() override {}
	virtual void Load(ResourcesManager::id_t File) override {}

	virtual void SetVolume(float Volume) override {};
};

std::unique_ptr<std::thread> TickInternal = {};

Device* pDevice = nullptr;

void Audio::Init()
{
#ifdef OS_WINDOWS
	pDevice = new DeviceXAudio2;
#else 
	pDevice = new DeviceOpenAL;
#endif

	TickInternal = std::make_unique<std::thread>([]()
	{
		Threads::SetName("Asura Audio: Tick");
		while (true)
		{
			if (pDevice == nullptr)
				return;

			pDevice->Tick();

			//Threads::Wait();
		}
	});
	Threads::SetAffinity(*TickInternal.get(), 6);
}

void Audio::Tick()
{
}

void Audio::Destroy()
{
	if (pDevice == nullptr)
		return;

	delete pDevice;
	pDevice = nullptr;
}

void Audio::Reset(DeviceMode Mode)
{
	Destroy();

	switch (Mode)
	{
	case DeviceMode::eOpenAL:	pDevice = new DeviceOpenAL;
#ifdef OS_WINDOWS
	case DeviceMode::eXAudio2:	pDevice = new DeviceXAudio2;
#endif
	default:					pDevice = new DeviceDummy;
	}
}

void Asura::Audio::UpdateVolume()
{
	if (pDevice == nullptr)
		return;

	pDevice->SetVolume(Volume);
}

void Audio::Start(stl::string_view File)
{
	if (pDevice == nullptr)
		return;

	FileSystem::Path FilePath = "sound";
	FilePath.append(File);

	pDevice->Load(ResourcesManager::Load(FilePath));
}