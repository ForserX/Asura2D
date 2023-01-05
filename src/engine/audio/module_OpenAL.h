#pragma once
namespace al_trash
{
	struct stream_audio_data;
};

namespace Asura::Audio
{
	class DeviceOpenAL final : public Device
	{
		stl::vector<al_trash::stream_audio_data*> AudioData;
		stl::vector<ResourcesManager::id_t> IdRes;

		std::recursive_mutex SafeLock;
		std::recursive_mutex SafeLoaderLock;
	public:
		DeviceOpenAL();
		~DeviceOpenAL();

		virtual void Tick() override;
		virtual void PreTick() override {};
		virtual void Load(ResourcesManager::id_t File) override;

		virtual void SetVolume(float Volume) override;
	};
};
