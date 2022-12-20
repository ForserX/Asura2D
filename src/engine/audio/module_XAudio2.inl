constexpr size_t STREAMING_BUFFER_SIZE = 65536;
constexpr size_t MAX_BUFFER_COUNT = 3;

template <class T>
void SafeRelease(T pInterface)
{
	if (pInterface != nullptr) 
	{ 
		pInterface->Release();
		pInterface = nullptr; 
	}
}

int cb_read(void* stream, unsigned char* ptr, int nbytes) 
{
	std::fstream* in = (std::fstream*)stream;
	//nassertr(in != nullptr, -1);

	in->read((char*)ptr, nbytes);

	if (in->eof())
	{
		// Gracefully handle EOF.
		in->clear();
	}

	return in->gcount();
}

using Asura::Audio::Decoder::OpusDecoderInfo;

class CAudio final
{
public:
	CAudio();
	virtual ~CAudio();

	bool IsPlaying();
	void Stop();
	bool Play(bool loop = true);
	bool LoadSound(OpusDecoderInfo& DecInfo);
	void AlterVolume(float fltVolume);
	float GetVolume();
	void Pause();
	void Update();

public:
	bool bDone;

private:
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	IXAudio2SourceVoice* pSourceVoice;

	UINT32 flags;
	OpusDecoderInfo DecInfo;

	bool isRunning;
	bool boolIsPaused;
	bool bAlmostDone;
	bool bLoop;

	void resetParams();
};

CAudio::CAudio(void)
{
	pXAudio2 = nullptr;
	pMasteringVoice = nullptr;
	pSourceVoice = nullptr;

	resetParams();

#ifdef ASURA_XAUDIO_DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	HRESULT hr = S_OK;

	if (FAILED(hr = XAudio2Create(&pXAudio2, flags)))
	{
		Asura::Debug::msg("Failed to init XAudio2 engine: {}", hr);
		return;
	}

	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
	{
		Asura::Debug::msg("Failed creating mastering voice : {}", hr);
		return;
	}
}

CAudio::~CAudio(void)
{
	if (pSourceVoice != nullptr)
	{
		pSourceVoice->Stop(0);
		pSourceVoice->DestroyVoice();
	}

	if (pMasteringVoice != nullptr)
		pMasteringVoice->DestroyVoice();

	SafeRelease(pXAudio2);
}

void CAudio::resetParams()
{
	isRunning = false;
	boolIsPaused = false;
	bLoop = false;
	bDone = false;
	bAlmostDone = false;
	flags = 0;
}

bool CAudio::LoadSound(Asura::Audio::Decoder::OpusDecoderInfo& InDecInfo)
{
	errno_t err;

	memcpy(&DecInfo, &InDecInfo, sizeof(OpusDecoderInfo));

	//Set the wave format
	static WAVEFORMATEX wfm;

	wfm.cbSize = sizeof(wfm);
	wfm.nChannels = DecInfo.ChannelsCount;
	wfm.wBitsPerSample = 16;
	wfm.nSamplesPerSec = DecInfo.Rate;
	wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nChannels * 2;
	wfm.nBlockAlign = 2 * wfm.nChannels;
	wfm.wFormatTag = 1;

	HRESULT hr;

	//Create the source voice
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wfm)))
	{
		Asura::Debug::msg(">Error %#X creating source voice");
		//LogError("<li>Error %#X creating source voice", hr);
		return false;
	}

	//Submit the wave sample data using an XAUDIO2_BUFFER structure
	static XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = (BYTE*)&DecInfo.buffers;
	buffer.AudioBytes = STREAMING_BUFFER_SIZE;

	if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		Asura::Debug::msg("Error %#X submitting source buffer");
		return false;
	}

	return true;
}

bool CAudio::Play(bool loop)
{
	if (pSourceVoice == nullptr)
	{
		//LogError("<li>Error: pSourceVoice NOT created");
		return false;
	}

	HRESULT hr;

	if (FAILED(hr = pSourceVoice->Start(0)))
	{
		//LogError("<li>Error %#X submitting source buffer", hr);
	}

	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);
	isRunning = (state.BuffersQueued > 0) != 0;

	bLoop = loop;
	bDone = false;
	bAlmostDone = false;
	boolIsPaused = false;

	return isRunning;
}

void CAudio::Stop()
{
	if (pSourceVoice == nullptr)
		return;

	pSourceVoice->Stop(0);

	boolIsPaused = false;
	isRunning = false;
}

bool CAudio::IsPlaying()
{
	return isRunning;
}

void CAudio::AlterVolume(float fltVolume)
{
	if (pSourceVoice == nullptr)
		return;

	pSourceVoice->SetVolume(fltVolume);
}

float CAudio::GetVolume()
{
	float Base = 0.f;

	if (pSourceVoice == nullptr)
		return Base;

	pSourceVoice->GetVolume(&Base);
	return Base;
}

void CAudio::Pause()
{
	if (pSourceVoice == nullptr)
		return;

	if (boolIsPaused)
	{
		pSourceVoice->Start(0);
		boolIsPaused = false;
	}
	else
	{
		pSourceVoice->Stop(0);
		boolIsPaused = true;
	}
}

void CAudio::Update()
{
	if (pSourceVoice == nullptr)
		return;

	if (!isRunning)
		return;

	//Do we have any free buffers?
	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);
	if (state.BuffersQueued < MAX_BUFFER_COUNT - 1)
	{
		if (bDone && !bLoop)
		{
			pSourceVoice->Stop(0);
		}

		if(bAlmostDone)
			bDone = true;

		memset(&DecInfo.buffers, 0, sizeof(DecInfo.buffers));

		DWORD pos = 0;
		int sec = 0;
		int ret = 1;

		while (ret && pos < STREAMING_BUFFER_SIZE)
		{
			ret = op_read(DecInfo.vf, DecInfo.buffers + pos, STREAMING_BUFFER_SIZE - pos, &sec);
			pos += ret;
		}

		//Reached the end?
		if (!ret && bLoop)
		{
			//We are looping so restart from the beginning
			//NOTE: sound with sizes smaller than BUFSIZE may be cut off

			ret = 1;
			op_pcm_seek(DecInfo.vf, 0);
			while (ret && pos < STREAMING_BUFFER_SIZE)
			{
				ret = op_read(DecInfo.vf, DecInfo.buffers + pos, STREAMING_BUFFER_SIZE - pos, &sec);
				pos += ret;
			}
		}
		else if (!ret && !(bLoop))
		{
			bAlmostDone = true;
		}

		XAUDIO2_BUFFER buffer = { 0 };
		buffer.pAudioData = (BYTE*)&DecInfo.buffers;
		if (bAlmostDone)
			buffer.Flags = XAUDIO2_END_OF_STREAM;	//Tell the source voice not to expect any data after this buffer
		buffer.AudioBytes = STREAMING_BUFFER_SIZE;

		HRESULT hr;
		if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
		{
			//LogError("<li>Error %#X submitting source buffer\n", hr );
			return;
		}
	}
}