#include "pch.h"

using namespace Asura;

stl::hash_map<ResourcesManager::id_t, Audio::Decoder::OpusDecoderInfo*> ODList;

namespace Asura::Audio::Internal
{
	using FileSystem::Reader;

	int DecSeak(void* datasource, int64_t offset, int whence)
	{
		Reader* Data = ((Reader*)datasource);

		switch (whence)
		{
		case SEEK_SET:
			Data->Seek(offset);
			break;
		case SEEK_CUR:
			Data->Move(offset);
			break;
		case SEEK_END:
			Data->Seek((size_t)offset + Data->Lenght());
			break;
		}
		return 0;
	}

	int DecRead(void* datasource, unsigned char* ptr, int nmemb)
	{
		Reader* F = (Reader*)datasource;
		size_t exist_block = std::max(size_t(0), F->Elapsed());
		size_t read_block = std::min(exist_block, (size_t)nmemb);

		if (read_block > 0)
		{
			F->Get(ptr, read_block);
		}
		return read_block;
	}

	int DecClose(void* datasource)
	{
		delete((Reader*)datasource);

		return 0;
	}

	int64_t DecTell(void* datasource)
	{
		return ((Reader*)datasource)->Tell();
	}
}


void Audio::Decoder::Init()
{
}

void Audio::Decoder::Destroy()
{
	for (auto DecInfo : ODList)
	{
		op_free(DecInfo.second->vf);
		delete DecInfo.second;
	}

	ODList.clear();
}

void Audio::Decoder::Load(ResourcesManager::id_t ResID)
{
	if (ODList[ResID] != nullptr)
	{
		return;
	}

	Resource Res = ResourcesManager::GetResource(ResID);

	OpusFileCallbacks ovc = { Internal::DecRead, Internal::DecSeak, Internal::DecTell , Internal::DecClose };
	OpusDecoderInfo* DecInfo = new OpusDecoderInfo;

	FileSystem::Reader* pReader = new FileSystem::Reader(Res.Name);

	int ErrorCode = 0;

	DecInfo->vf = op_open_callbacks(pReader, &ovc, nullptr, 0, &ErrorCode);
	game_assert(DecInfo->vf, Res.Name, return);

	int sec = 0;
	int ret = 1;

	memset(&DecInfo->buffers, 0, sizeof(DecInfo->buffers));

	int li = op_current_link(DecInfo->vf);
	const OpusHead* head = op_head(DecInfo->vf, li);

	DecInfo->Rate = head->input_sample_rate;
	DecInfo->ChannelsCount = (uint16)head->channel_count;

	//Read in the bits
	while (ret && DecInfo->Pos < 65536)
	{
		ret = op_read(DecInfo->vf, DecInfo->buffers + DecInfo->Pos, 65536 - DecInfo->Pos, &sec);
		DecInfo->Pos += ret * DecInfo->ChannelsCount;
	}

	ODList[ResID] = DecInfo;
}

Audio::Decoder::OpusDecoderInfo Audio::Decoder::Get(ResourcesManager::id_t ResID)
{
	if (ODList[ResID] == nullptr)
	{
		Load(ResID);
	}

	op_pcm_seek(ODList[ResID]->vf, 0);

	return *ODList[ResID];
}