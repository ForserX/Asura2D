#pragma once

namespace Asura::Audio::Decoder
{
	struct OpusDecoderInfo
	{
		OggOpusFile* vf = nullptr;
		opus_int16 buffers[65536];

		std::uint16_t ChannelsCount = 0;
		std::uint32_t Rate = 44000;
	};

	void Init();
	void Destroy();

	void Load(ResourcesManager::id_t ResID);
	OpusDecoderInfo Get(ResourcesManager::id_t ResID);
}