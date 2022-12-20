#pragma once

class CAudio;

namespace Asura::Audio
{
	class DeviceXAudio2 final : public Device
	{
		stl::vector<CAudio*> AudioData;
		stl::vector<ResourcesManager::id_t> IdRes;
	private:
		stl::vector<ResourcesManager::id_t>& GetSafe();
	public:
		DeviceXAudio2();
		~DeviceXAudio2();

		virtual void Tick() override;
		virtual void Load(ResourcesManager::id_t File) override;

		virtual void SetVolume(float Volume) override;
	};
};
