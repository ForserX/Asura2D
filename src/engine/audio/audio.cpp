#include "pch.h"
#include "module_OpenAL.h"

#ifdef OS_WINDOWS
#include "module_XAudio2.h"
#endif

using namespace Asura;
using Asura::Audio::Device;

float Volume = 1.f;
volatile bool bDestroy = false;

class DeviceDummy : public Device
{
public:
	DeviceDummy() = default;
	~DeviceDummy() = default;

	virtual void Tick() override {}
	virtual void PreTick() override {};
	virtual void Load(ResourcesManager::id_t File) override {}

	virtual void SetVolume(float Volume) override {};
};

std::unique_ptr<std::thread> TickInternal = {};
std::unique_ptr<std::thread> PreTickInternal = {};

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
		
		volatile Threads::ScopeCOThread COData;

		while (true)
		{
			Threads::Wait();

			if (bDestroy)
			{
				break;
			}

			pDevice->Tick();
		}

		delete pDevice;
		pDevice = nullptr;

		bDestroy = false;
	});

	Threads::SetAffinity(*TickInternal.get(), 4);
}

void Audio::Tick()
{
}

void Audio::Destroy()
{
	if (pDevice == nullptr)
		return;

	bDestroy = true;
	TickInternal->detach();

	while (bDestroy)
	{
		Threads::Wait();
	}
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

	auto Res = ResourcesManager::Load(FilePath);

	game_assert(Res, "incorrect file/path", return);
	pDevice->Load(Res.Get());
}