#include "pch.h"

#include "module_OpenAL.h"

using namespace Asura;

static bool al_work = false;

namespace al_trash
{
#	include "module_OpenAL.inl"
}

Audio::DeviceOpenAL::DeviceOpenAL()
{
	al_work = !al_trash::init_al();
	AudioData = {};
}

void Audio::DeviceOpenAL::Tick()
{
	if (!al_work)
	{
		return;
	}

	if (!IdRes.empty())
	{
		std::lock_guard Lock(SafeLoaderLock);

		for (const ResourcesManager::id_t& SoundSrc : IdRes)
		{
			auto DecInfo = Decoder::Get(SoundSrc);

			al_trash::stream_audio_data& ref_data = *AudioData.emplace_back(new al_trash::stream_audio_data);
			al_trash::create_stream_from_file(DecInfo, ref_data);

			al_trash::play_stream(ref_data);
			Threads::Wait();
		}

		IdRes.clear();
	}

	std::lock_guard Lock(SafeLock);

	for (al_trash::stream_audio_data* it : AudioData)
	{
		bool playing = al_trash::update_stream(*it);

		if (!playing)
		{
			auto iter = std::find(AudioData.begin(), AudioData.end(), it);
			AudioData.erase(iter);
			ALDestroyStream(*it);
			delete it;
		}
	}
}

Audio::DeviceOpenAL::~DeviceOpenAL()
{
	if (!al_work) 
	{
		return;
	}

	for (al_trash::stream_audio_data* it : AudioData)
	{
		delete it;
	}

	AudioData.clear();
	al_trash::CloseAL();
}

void Audio::DeviceOpenAL::Load(ResourcesManager::id_t SoundSrc)
{
	if (!al_work)
	{
		return;
	}

	std::lock_guard Lock(SafeLoaderLock);
	IdRes.push_back(SoundSrc);
}

void Asura::Audio::DeviceOpenAL::SetVolume(float Volume)
{
	// Use global value into Update call
}