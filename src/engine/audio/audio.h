#pragma once

#if defined(ASURA_OPEN_AL) | !defined(OS_WINDOWS)
#include "module_OpenAL.h"

namespace Asura::audio
{
	using namespace audio::openal;
}
#else

#include "module_XAudio2.h"
namespace Asura::audio
{
	using namespace audio::xaudio2;
}
#endif