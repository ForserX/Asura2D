#include "pch.h"
#include "module_OpenAL.h"

#include <openal-soft/include/AL/al.h>
#include <openal-soft/include/AL/alc.h>
#include <vorbis/include/vorbis/vorbisfile.h>
#include <ogg/include/ogg/os_types.h>
#include <ogg/include/ogg/ogg.h>

using namespace ark;

namespace al_trash
{
#	include "module_OpenAL.inl"
}

static stl::vector<al_trash::stream_audio_data*> data;

void audio::openal::init()
{
	al_trash::init_al();
	data = {};
}

void audio::openal::tick()
{
	for (al_trash::stream_audio_data* it : data)
	{
		bool playing = al_trash::update_stream(*it);

		if (!playing)
		{
			auto iter = std::find(data.begin(), data.end(), it);
			data.erase(iter);
			delete it;
		}
	}
}

void audio::openal::destroy()
{
	for (al_trash::stream_audio_data* it : data)
	{
		delete it;
	}

	data.clear();

	al_trash::CloseAL();
}

void audio::openal::start(stl::string_view sound_src)
{
	al_trash::stream_audio_data& ref_data = *data.emplace_back(new al_trash::stream_audio_data);
	al_trash::create_stream_from_file(sound_src, ref_data);
	
	al_trash::play_stream(ref_data);
}