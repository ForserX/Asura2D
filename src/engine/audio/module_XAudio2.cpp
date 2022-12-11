#include "pch.h"

#include "module_XAudio2.h"

#ifdef OS_WINDOWS
#include "module_XAudio2.inl"

using namespace Asura;
static stl::vector<CAudio*> AudioData;

void audio::xaudio2::Init()
{
}

void audio::xaudio2::Tick()
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

void audio::xaudio2::Destroy()
{
	for (CAudio* Audio : AudioData)
	{
		Audio->Stop();
		delete Audio;
	}

	AudioData.clear();
}

void audio::xaudio2::start(stl::string_view sound_src)
{
	CAudio* Audio = AudioData.emplace_back(new CAudio);

	Audio->LoadSound(sound_src.data());
	Audio->Play(false);
}

#endif