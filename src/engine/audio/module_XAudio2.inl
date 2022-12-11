constexpr size_t STREAMING_BUFFER_SIZE = 65536 * 10;
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

class CAudio final
{
public:
	CAudio();
	virtual ~CAudio();

	bool IsPlaying();
	void Stop();
	bool Play(bool loop = true);
	bool LoadSound(const char* szSoundFilePath);
	void AlterVolume(float fltVolume);
	void GetVolume(float& fltVolume);
	void Pause();
	void Update();

public:
	bool bDone;

private:
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	IXAudio2SourceVoice* pSourceVoice;

	UINT32 flags;
	char buffers[MAX_BUFFER_COUNT][STREAMING_BUFFER_SIZE];
	bool bFileOpened;
	OggVorbis_File vf;
	bool isRunning;
	bool boolIsPaused;
	bool bAlmostDone;
	bool bLoop;
	DWORD currentDiskReadBuffer;

	void resetParams();
};

CAudio::CAudio(void)
{
	pXAudio2 = nullptr;
	pMasteringVoice = nullptr;
	pSourceVoice = nullptr;

	resetParams();

#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

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

	if (bFileOpened)
		ov_clear(&vf);

	CoUninitialize();
}

void CAudio::resetParams()
{
	bFileOpened = false;
	isRunning = false;
	boolIsPaused = false;
	bLoop = false;
	bDone = false;
	bAlmostDone = false;
	currentDiskReadBuffer = 0;
	flags = 0;
}

bool CAudio::LoadSound(const char* strSoundPath)
{
	//If we already have a file open then kill the current voice setup
	if (bFileOpened)
	{
		pSourceVoice->Stop(0);
		pSourceVoice->DestroyVoice();

		ov_clear(&vf);

		resetParams();
	}

	FILE* f = nullptr;
	errno_t err;

	if ((err = fopen_s(&f, strSoundPath, "rb")) != 0)
	{
		Asura::Debug::msg("Failed to open audio: {}", strSoundPath);

		char szBuffer[MAX_PATH];
		_strerror_s(szBuffer, MAX_PATH, nullptr);
		Asura::Debug::msg("Reason: {}", szBuffer);
		return false;
	}

	if (ov_open_callbacks(f, &vf, nullptr, 0, OV_CALLBACKS_DEFAULT) < 0)
	{
		fclose(f);
		return false;
	}
	else
	{
		bFileOpened = true;
	}

	//The vorbis_info struct keeps the most of the interesting format info
	vorbis_info* vi = ov_info(&vf, -1);

	//Set the wave format
	WAVEFORMATEX wfm;
	memset(&wfm, 0, sizeof(wfm));

	wfm.cbSize = sizeof(wfm);
	wfm.nChannels = vi->channels;
	wfm.wBitsPerSample = 16;                    //Ogg vorbis is always 16 bit
	wfm.nSamplesPerSec = vi->rate;
	wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nChannels * 2;
	wfm.nBlockAlign = 2 * wfm.nChannels;
	wfm.wFormatTag = 1;

	DWORD pos = 0;
	int sec = 0;
	int ret = 1;

	memset(&buffers[currentDiskReadBuffer], 0, sizeof(buffers[currentDiskReadBuffer]));

	//Read in the bits
	while (ret && pos < STREAMING_BUFFER_SIZE)
	{
		ret = ov_read(&vf, buffers[currentDiskReadBuffer] + pos, STREAMING_BUFFER_SIZE - pos, 0, 2, 1, &sec);
		pos += ret;
	}

	HRESULT hr;

	//Create the source voice
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wfm)))
	{
		//LogError("<li>Error %#X creating source voice", hr);
		return false;
	}

	//Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = (BYTE*)&buffers[currentDiskReadBuffer];
	buffer.AudioBytes = STREAMING_BUFFER_SIZE;

	if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		//LogError("<li>Error %#X submitting source buffer", hr);
		return false;
	}

	currentDiskReadBuffer++;

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

	//XAUDIO2_FLUSH_BUFFERS according to MSDN is meant to flush the buffers after the voice is stopped
	//unfortunately the March 2008 release of the SDK does not include this parameter in the xaudio files
	//and I have been unable to ascertain what its value is
	//pSourceVoice->Stop(XAUDIO2_FLUSH_BUFFERS);
	pSourceVoice->Stop(0);

	boolIsPaused = false;
	isRunning = false;
}

bool CAudio::IsPlaying()
{
	/*XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);
	return (state.BuffersQueued > 0) != 0;*/

	return isRunning;
}


//Alter the volume up and down
void CAudio::AlterVolume(float fltVolume)
{
	if (pSourceVoice == nullptr)
		return;

	pSourceVoice->SetVolume(fltVolume);			//Current voice volume
	//pMasteringVoice->SetVolume(fltVolume);	//Playback device volume
}

//Return the current volume
void CAudio::GetVolume(float& fltVolume)
{
	if (pSourceVoice == nullptr)
		return;

	pSourceVoice->GetVolume(&fltVolume);
	//pMasteringVoice->GetVolume(&fltVolume);
}

void CAudio::Pause()
{
	if (pSourceVoice == nullptr)
		return;

	if (boolIsPaused)
	{
		pSourceVoice->Start(0);	//Unless we tell it otherwise the voice resumes playback from its last position
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

		//Got to use this trick because otherwise all the bits wont play
		if (bAlmostDone && !bLoop)
			bDone = true;

		memset(&buffers[currentDiskReadBuffer], 0, sizeof(buffers[currentDiskReadBuffer]));

		DWORD pos = 0;
		int sec = 0;
		int ret = 1;

		while (ret && pos < STREAMING_BUFFER_SIZE)
		{
			ret = ov_read(&vf, buffers[currentDiskReadBuffer] + pos, STREAMING_BUFFER_SIZE - pos, 0, 2, 1, &sec);
			pos += ret;
		}

		//Reached the end?
		if (!ret && bLoop)
		{
			//We are looping so restart from the beginning
			//NOTE: sound with sizes smaller than BUFSIZE may be cut off

			ret = 1;
			ov_pcm_seek(&vf, 0);
			while (ret && pos < STREAMING_BUFFER_SIZE)
			{
				ret = ov_read(&vf, buffers[currentDiskReadBuffer] + pos, STREAMING_BUFFER_SIZE - pos, 0, 2, 1, &sec);
				pos += ret;
			}
		}
		else if (!ret && !(bLoop))
		{
			//Not looping so fill the rest with 0
			//while(pos<size)
			//    *(buffers[currentDiskReadBuffer]+pos)=0; pos ++;

			//And say that after the current section no other section follows
			bAlmostDone = true;
		}

		XAUDIO2_BUFFER buffer = { 0 };
		buffer.pAudioData = (BYTE*)&buffers[currentDiskReadBuffer];
		if (bAlmostDone)
			buffer.Flags = XAUDIO2_END_OF_STREAM;	//Tell the source voice not to expect any data after this buffer
		buffer.AudioBytes = STREAMING_BUFFER_SIZE;

		HRESULT hr;
		if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
		{
			//LogError("<li>Error %#X submitting source buffer\n", hr );
			return;
		}

		currentDiskReadBuffer++;
		currentDiskReadBuffer %= MAX_BUFFER_COUNT;
	}
}