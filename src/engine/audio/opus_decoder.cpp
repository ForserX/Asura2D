#include "pch.h"

using namespace Asura;

stl::hash_map<ResourcesManager::id_t, Audio::Decoder::OpusDecoderInfo*> ODList;

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
	FileSystem::Path FullPath = FileSystem::ContentDir();
	FullPath.append(Res.Name);

	OpusDecoderInfo* DecInfo = new OpusDecoderInfo;

	DecInfo->vf = op_open_file(FullPath.generic_string().c_str(), {});

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
		DecInfo->Pos += ret;
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