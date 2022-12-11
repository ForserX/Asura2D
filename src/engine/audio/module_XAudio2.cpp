#include "pch.h"

#include "module_XAudio2.h"

#ifdef OS_WINDOWS
#include "module_XAudio2.inl"

using namespace asura;
static stl::vector<CAudio*> AudioData;

void audio::xaudio2::init()
{
}

void audio::xaudio2::tick()
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

void audio::xaudio2::destroy()
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