#include "pch.h"

#ifdef OS_WINDOWS
// FX: XAudio just it's DirectX Sound: Windows only

#include "module_XAudio2.h"
#include "module_XAudio2.inl"

using namespace Asura;

Audio::DeviceXAudio2::DeviceXAudio2()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

Audio::DeviceXAudio2::~DeviceXAudio2()
{
	for (CAudio* Audio : AudioData)
	{
		Audio->Stop();
		delete Audio;
	}

	AudioData.clear();

	CoUninitialize();
}

void Audio::DeviceXAudio2::PreTick()
{
}

void Audio::DeviceXAudio2::Tick()
{
	if (!IdRes.empty())
	{
		CAudio* Audio = nullptr;

		SafeLoaderLock.lock();

		for (const ResourcesManager::id_t& ID : IdRes)
		{
			auto DecInfo = Decoder::Get(ID);

			Audio = new CAudio;
			Audio->LoadSound(DecInfo);
			Audio->AlterVolume(Volume);
			Audio->Play(false);

			Threads::Wait();
			AudioData.emplace_back(Audio);
		}

		IdRes.clear();
		SafeLoaderLock.unlock();
	}

	std::lock_guard Lock(SafeLock);

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

void Audio::DeviceXAudio2::Load(ResourcesManager::id_t SoundSrc)
{
	std::lock_guard Lock(SafeLoaderLock);
	IdRes.push_back(SoundSrc);
}

void Asura::Audio::DeviceXAudio2::SetVolume(float InVolume)
{
	std::lock_guard Lock(SafeLock);

	for (CAudio* pData : AudioData)
	{
		pData->AlterVolume(InVolume);
	}
}

#endif