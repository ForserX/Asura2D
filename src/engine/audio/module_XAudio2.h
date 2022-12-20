#pragma once

class CAudio;

namespace Asura::Audio
{
	class DeviceXAudio2 final : public Device
	{
		stl::vector<CAudio*> AudioData;
		stl::vector<ResourcesManager::id_t> IdRes;

		std::recursive_mutex SafeLock;
		std::recursive_mutex SafeLoaderLock;
		std::recursive_mutex SafeTempLoaderLock;

	private:
		inline stl::vector<ResourcesManager::id_t>& GetSafe() { std::lock_guard lock(SafeLoaderLock); return IdRes; };
	public:
		DeviceXAudio2();
		~DeviceXAudio2();

		virtual void PreTick() override;
		virtual void Tick() override;
		virtual void Load(ResourcesManager::id_t File) override;

		virtual void SetVolume(float Volume) override;
	};
};
