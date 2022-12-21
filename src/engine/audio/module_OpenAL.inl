
constexpr size_t NUM_BUFFERS = 4;
constexpr ALsizei BUFFER_SIZE = 65536;
using Asura::Audio::Decoder::OpusDecoderInfo;

struct stream_audio_data
{
    ALuint buffers[NUM_BUFFERS];
    uint8 channels;
    int32 sampleRate;
    uint8 bitsPerSample;
    ALsizei size;
    ALuint source;
    ALsizei size_consumed = 0;
    ALenum format;

    OpusDecoderInfo DecInfo;

    int ogg_current_section = 0;
    size_t duration;
};

int init_al()
{
    const ALCchar* name;
    ALCdevice* device;
    ALCcontext* ctx;

    /* Open and initialize a device */
    device = alcOpenDevice(nullptr);
    if (!device)
    {
        Debug::msg("Could not open a device!");
        return 1;
    }

    ctx = alcCreateContext(device, nullptr);
    if (ctx == nullptr || alcMakeContextCurrent(ctx) == ALC_FALSE)
    {
        if (ctx != nullptr)
            alcDestroyContext(ctx);

        alcCloseDevice(device);
        Debug::msg("Could not set a context!");
        return 1;
    }

    name = nullptr;
    if (alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);

    if (!name || alcGetError(device) != AL_NO_ERROR)
        name = alcGetString(device, ALC_DEVICE_SPECIFIER);

    Debug::msg("Opened {}", name);

    return 0;
}

void CloseAL(void)
{
    ALCdevice* device;
    ALCcontext* ctx;

    ctx = alcGetCurrentContext();
    if (ctx == NULL)
        return;

    device = alcGetContextsDevice(ctx);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(device);
}

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

void check_al_errors(const stl::string_view filename, const std::uint_fast32_t line)
{
    ALCenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        Debug::msg("AL: {} {}", filename, line);
        switch (error)
        {
        case AL_INVALID_NAME:
            Debug::msg("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
            break;
        case AL_INVALID_ENUM:
            Debug::msg("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
            break;
        case AL_INVALID_VALUE:
            Debug::msg("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
            break;
        case AL_INVALID_OPERATION:
            Debug::msg("AL_INVALID_OPERATION: the requested operation is not valid");
            break;
        case AL_OUT_OF_MEMORY:
            Debug::msg("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
            break;
        default:
            Debug::msg("UNKNOWN AL ERROR: {} ", error);
        }
    }
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params)
->typename std::enable_if<std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
{
    function(std::forward<Params>(params)...);
    check_al_errors(filename, line);
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params)
->typename std::enable_if<!std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
{
    auto ret = function(std::forward<Params>(params)...);
    check_al_errors(filename, line);
    return ret;
}

bool create_stream_from_file(OpusDecoderInfo Dec, stream_audio_data& audio_data)
{
    audio_data.size_consumed = 0;

    memcpy(&audio_data.DecInfo, &Dec, sizeof(OpusDecoderInfo));

    audio_data.channels = Dec.ChannelsCount;
    audio_data.bitsPerSample = 16;
    audio_data.sampleRate = Dec.Rate;
    audio_data.duration = op_pcm_total(Dec.vf, -1);

    alCall(alGenSources, 1, &audio_data.source);
    alCall(alSourcef, audio_data.source, AL_GAIN, Volume);
    alCall(alSourcef, audio_data.source, AL_PITCH, 1);
    alCall(alSource3f, audio_data.source, AL_POSITION, 0, 0, 0);
    alCall(alSource3f, audio_data.source, AL_VELOCITY, 0, 0, 0);
    alCall(alSourcei, audio_data.source, AL_LOOPING, AL_FALSE);

    alCall(alGenBuffers, 1, &audio_data.buffers[0]);
    audio_data.format = AL_FORMAT_STEREO16;

    alCall(alBufferData, audio_data.buffers[0], audio_data.format, &audio_data.DecInfo.buffers, audio_data.DecInfo.Pos, audio_data.sampleRate);
    alCall(alSourceQueueBuffers, audio_data.source, 1, &audio_data.buffers[0]);

    return true;
}

bool update_stream(stream_audio_data& audio_data)
{
    ALint buffersProcessed = 0;
    alCall(alGetSourcei, audio_data.source, AL_BUFFERS_PROCESSED, &buffersProcessed);
    if (buffersProcessed <= 0)
    {
        return true;
    }
    while (buffersProcessed--)
    {
        ALuint buffer;
        alCall(alSourceUnqueueBuffers, audio_data.source, 1, &buffer);

        opus_int16* data = new opus_int16[BUFFER_SIZE];
        std::memset(data, 0, BUFFER_SIZE);

        ALsizei dataSizeToBuffer = 0;
        std::int32_t sizeRead = 0;

        while (sizeRead < BUFFER_SIZE - 1)
        {
            std::int32_t result = op_read(audio_data.DecInfo.vf, &data[sizeRead], BUFFER_SIZE - sizeRead, &audio_data.ogg_current_section);
            if (result == OP_HOLE)
            {
                // std::cerr << "ERROR: OV_HOLE found in update of buffer " << std::endl;
                break;
            }
            else if (result == OP_EBADLINK)
            {
                Asura::Debug::msg("ERROR: OV_EBADLINK found in update of buffer {}", sizeRead);
                //  std::cerr << "ERROR: OV_EBADLINK found in update of buffer " << std::endl;
                break;
            }
            else if (result == OP_EINVAL)
            {
                // std::cerr << "ERROR: OV_EINVAL found in update of buffer " << std::endl;
                break;
            }
            else if (result == 0)
            {
                std::int32_t seekResult = op_raw_seek(audio_data.DecInfo.vf, 0);

                //if (seekResult == OV_ENOSEEK)
                //    std::cerr << "ERROR: OV_ENOSEEK found when trying to loop" << std::endl;
                //else if (seekResult == OV_EINVAL)
                //    std::cerr << "ERROR: OV_EINVAL found when trying to loop" << std::endl;
                //else if (seekResult == OV_EREAD)
                //    std::cerr << "ERROR: OV_EREAD found when trying to loop" << std::endl;
                //else if (seekResult == OV_EFAULT)
                //    std::cerr << "ERROR: OV_EFAULT found when trying to loop" << std::endl;
                //else if (seekResult == OV_EOF)
                //    std::cerr << "ERROR: OV_EOF found when trying to loop" << std::endl;
                //else if (seekResult == OV_EBADLINK)
                //    std::cerr << "ERROR: OV_EBADLINK found when trying to loop" << std::endl;

                if (seekResult != 0)
                {
                    // std::cerr << "ERROR: Unknown error in ov_raw_seek" << std::endl;
                    return false;
                }
            }
            sizeRead += result;
        }
        dataSizeToBuffer = sizeRead;

        if (dataSizeToBuffer > 0)
        {
            alCall(alBufferData, buffer, audio_data.format, data, dataSizeToBuffer, audio_data.sampleRate);
            alCall(alSourceQueueBuffers, audio_data.source, 1, &buffer);
        }

        if (dataSizeToBuffer < BUFFER_SIZE)
        {
            // std::cout << "Data missing" << std::endl;
        }

        delete[] data;

        ALint state;
        alCall(alGetSourcei, audio_data.source, AL_SOURCE_STATE, &state);

        if (state == AL_STOPPED)
        {
           // op_free(audio_data.DecInfo.vf);
            return false;
        }

        if (state != AL_PLAYING)
        {
            alCall(alSourceStop, audio_data.source);
            alCall(alSourcePlay, audio_data.source);
        }
    }

    return true;
}

void play_stream(const stream_audio_data& audio_data)
{
    alCall(alSourceStop, audio_data.source);
    alCall(alSourcePlay, audio_data.source);
}