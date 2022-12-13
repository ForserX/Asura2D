#include "pch.h"

#ifdef OS_WINDOWS
// FX: XAudio just it's DirectX Sound: Windows only

#include "module_XAudio2.h"
#include "module_XAudio2.inl"

using namespace Asura;

Audio::DeviceXAudio2::DeviceXAudio2()
{
}

Audio::DeviceXAudio2::~DeviceXAudio2()
{
	for (CAudio* Audio : AudioData)
	{
		Audio->Stop();
		delete Audio;
	}

	AudioData.clear();
}

void Audio::DeviceXAudio2::Tick()
{
	for (CAudio* Audio : AudioData)
	{
		Audio->Update();

		if (Audio->bDone)
		{
			auto It = std::find(AudioData.begin(), AudioData.end(), Audio);
			AudioData.erase(It);
			delete Audio;
		}
	}
}

void Audio::DeviceXAudio2::Load(ResourcesManager::id_t sound_src)
{
	CAudio* Audio = AudioData.emplace_back(new CAudio);
	Resource Res = ResourcesManager::GetResource(sound_src);

	std::filesystem::path FullPath = FileSystem::ContentDir();
	FullPath.append(Res.Name);
	
	Audio->LoadSound(ExtractPath(FullPath).data());
	Audio->Play(false);
	Audio->AlterVolume(Volume);
}

void Asura::Audio::DeviceXAudio2::SetVolume(float InVolume)
{
	for (CAudio* pData : AudioData)
	{
		pData->AlterVolume(InVolume);
	}
}

#endif