#include "pch.h"

#ifdef OS_WINDOWS
// FX: XAudio just it's DirectX Sound: Windows only

#include "module_XAudio2.h"
#include "module_XAudio2.inl"

using namespace Asura;

std::recursive_mutex SafeLock;
std::recursive_mutex SafeLoadLock;
std::unique_ptr<std::thread> LoadThread;

Audio::DeviceXAudio2::DeviceXAudio2()
{
	LoadThread = std::make_unique<std::thread>([this]
	{
		Threads::SetName("Asura Audio: Locader");
		while (true)
		{
			if (IdRes.empty())
				Threads::Wait();

			for (auto SoundSrc : GetSafe())
			{
				Resource Res = ResourcesManager::GetResource(SoundSrc);
				FileSystem::Path FullPath = FileSystem::ContentDir();
				FullPath.append(Res.Name);

				SafeLock.lock();
				CAudio* Audio = AudioData.emplace_back(new CAudio);

				Audio->LoadSound(ExtractPath(FullPath).data());
				Audio->Play(false);
				Audio->AlterVolume(Volume);
				SafeLock.unlock();
			}

			std::lock_guard Lock(SafeLoadLock);
			IdRes.clear();
		}
	});

	Threads::SetAffinity(*LoadThread.get(), 4);
}

stl::vector<ResourcesManager::id_t>& Audio::DeviceXAudio2::GetSafe()
{
	std::lock_guard Lock(SafeLoadLock);
	return IdRes;
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

void Audio::DeviceXAudio2::Load(ResourcesManager::id_t sound_src)
{
	std::lock_guard Lock(SafeLoadLock);
	IdRes.push_back(sound_src);
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