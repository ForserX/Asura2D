#include "pch.h"
#include "module_OpenAL.h"

#ifdef OS_WINDOWS
#include "module_XAudio2.h"
#endif

using namespace Asura;
using Asura::Audio::Device;

class DeviceDummy : public Device
{
public:
	DeviceDummy() = default;
	~DeviceDummy() = default;

	virtual void Tick() override {}
	virtual void Load(ResourcesManager::id_t File) override {}
};

Device* pDevice = nullptr;

void Audio::Init()
{
#ifdef OS_WINDOWS
	pDevice = new DeviceXAudio2;
#else 
	pDevice = new DeviceOpenAL;
#endif
}

void Audio::Tick()
{
	if (pDevice == nullptr)
		return;

	pDevice->Tick();
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

void Audio::Start(stl::string_view File)
{
	if (pDevice == nullptr)
		return;

	std::filesystem::path FilePath = "sound";
	FilePath.append(File);

	pDevice->Load(ResourcesManager::Load(FilePath.generic_string().c_str()));
}