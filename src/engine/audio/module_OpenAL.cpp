#include "pch.h"
#include "module_OpenAL.h"

using namespace asura;

static bool al_work = false;

namespace al_trash
{
#	include "module_OpenAL.inl"
}

static stl::vector<al_trash::stream_audio_data*> data;

void audio::openal::init()
{
	al_work = !al_trash::init_al();
	data = {};
}

void audio::openal::tick()
{
	if (!al_work)
	{
		return;
	}

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
	if (!al_work) 
	{
		return;
	}

	for (al_trash::stream_audio_data* it : data)
	{
		delete it;
	}

	data.clear();
	al_trash::CloseAL();
}

void audio::openal::start(stl::string_view sound_src)
{
	if (!al_work) 
	{
		return;
	}

	al_trash::stream_audio_data& ref_data = *data.emplace_back(new al_trash::stream_audio_data);
	al_trash::create_stream_from_file(sound_src, ref_data);
	
	al_trash::play_stream(ref_data);
}