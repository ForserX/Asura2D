
constexpr size_t NUM_BUFFERS = 4;
constexpr ALsizei BUFFER_SIZE = 65536;

struct stream_audio_data
{
    ALuint buffers[NUM_BUFFERS];
    stl::string filename;
    std::ifstream file;
    uint8 channels;
    int32 sampleRate;
    uint8 bitsPerSample;
    ALsizei size;
    ALuint source;
    ALsizei size_consumed = 0;
    ALenum format;
    OggVorbis_File ogg_vorbis_file;
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

static stl::byte_vector audio_buffer;

size_t read_ogg_callback(void* destination, size_t size1, size_t size2, void* fileHandle)
{
    stream_audio_data* audio_data = reinterpret_cast<stream_audio_data*>(fileHandle);

    ALsizei length = size1 * size2;

    if (audio_data->size_consumed + length > audio_data->size)
    {
        length = audio_data->size - audio_data->size_consumed;
    }

    if (!audio_data->file.is_open())
    {
        audio_data->file.open(audio_data->filename, std::ios::binary);
        if (!audio_data->file.is_open())
        {
            Debug::msg("AL ERROR: Could not re - open streaming file {}", audio_data->filename);
            return 0;
        }
    }

    std::memset(audio_buffer.data(), 0, audio_buffer.size());
    audio_buffer.resize(length);

    audio_data->file.clear();
    audio_data->file.seekg(audio_data->size_consumed);

    if (!audio_data->file.read(audio_buffer.data(), length)) 
    {
        if (audio_data->file.eof()) {
            audio_data->file.clear(); // just clear the error, we will resolve it later
        }
        else if (audio_data->file.fail()) {
            Debug::msg("AL ERROR: OGG stream has fail bit set {} ", audio_data->filename);
            audio_data->file.clear();
            return 0;
        }
        else if (audio_data->file.bad())
        {
            Debug::msg("AL ERROR: OGG stream has bad bit set {}", audio_data->filename);
            audio_data->file.clear();
            return 0;
        }
    }

    audio_data->size_consumed += length;
    std::memcpy(destination, audio_buffer.data(), length);
    audio_data->file.clear();

    return length;
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

int seek_ogg_callback(void* fileHandle, ogg_int64_t to, int type)
{
    stream_audio_data* audio_data = reinterpret_cast<stream_audio_data*>(fileHandle);

    if (type == SEEK_CUR)
    {
        audio_data->size_consumed += to;
    }
    else if (type == SEEK_END)
    {
        audio_data->size_consumed = audio_data->size - to;
    }
    else if (type == SEEK_SET)
    {
        audio_data->size_consumed = to;
    }
    else
        return -1; // what are you trying to do vorbis?

    if (audio_data->size_consumed < 0)
    {
        audio_data->size_consumed = 0;
        return -1;
    }
    if (audio_data->size_consumed > audio_data->size)
    {
        audio_data->size_consumed = audio_data->size;
        return -1;
    }

    return 0;
}

long int tell_ogg_callback(void* fileHandle)
{
    stream_audio_data* audio_data = reinterpret_cast<stream_audio_data*>(fileHandle);
    return audio_data->size_consumed;
}

bool create_stream_from_file(stl::string_view filename, stream_audio_data& audio_data)
{
    audio_data.filename = filename;
    audio_data.file.open(filename.data(), std::ios::binary);
    if (!audio_data.file.is_open())
    {
        Debug::msg("AL ERROR: couldn't open file");
        return 0;
    }

    audio_data.file.seekg(0, std::ios_base::beg);
    audio_data.file.ignore(std::numeric_limits<std::streamsize>::max());
    audio_data.size = audio_data.file.gcount();
    audio_data.file.clear();
    audio_data.file.seekg(0, std::ios_base::beg);
    audio_data.size_consumed = 0;

    ov_callbacks oggCallbacks;
    oggCallbacks.read_func = read_ogg_callback;
    oggCallbacks.close_func = nullptr;
    oggCallbacks.seek_func = seek_ogg_callback;
    oggCallbacks.tell_func = tell_ogg_callback;

    if (ov_open_callbacks(reinterpret_cast<void*>(&audio_data), &audio_data.ogg_vorbis_file, nullptr, -1, oggCallbacks) < 0)
    {
        Debug::msg("AL ERROR: Could not ov_open_callbacks");
        return false;
    }

    vorbis_info* vorbisInfo = ov_info(&audio_data.ogg_vorbis_file, -1);

    audio_data.channels = vorbisInfo->channels;
    audio_data.bitsPerSample = 16;
    audio_data.sampleRate = vorbisInfo->rate;
    audio_data.duration = ov_time_total(&audio_data.ogg_vorbis_file, -1);

    alCall(alGenSources, 1, &audio_data.source);
    alCall(alSourcef, audio_data.source, AL_PITCH, 1);
    alCall(alSource3f, audio_data.source, AL_POSITION, 0, 0, 0);
    alCall(alSource3f, audio_data.source, AL_VELOCITY, 0, 0, 0);
    alCall(alSourcei, audio_data.source, AL_LOOPING, AL_FALSE);

    alCall(alGenBuffers, NUM_BUFFERS, &audio_data.buffers[0]);

    if (audio_data.file.eof())
    {
        Debug::msg("AL ERROR: Already reached EOF without loading data");
        return false;
    }
    else if (audio_data.file.fail())
    {
        Debug::msg("AL ERROR: Fail bit set");
        return false;
    }
    else if (!audio_data.file)
    {
        Debug::msg("AL ERROR: file is false");
        return false;
    }

    char* data = new char[BUFFER_SIZE];

    for (std::uint8_t i = 0; i < NUM_BUFFERS; ++i)
    {
        std::int32_t dataSoFar = 0;
        while (dataSoFar < BUFFER_SIZE)
        {
            std::int32_t result = ov_read(&audio_data.ogg_vorbis_file, &data[dataSoFar], BUFFER_SIZE - dataSoFar, 0, 2, 1, &audio_data.ogg_current_section);
            if (result == OV_HOLE)
            {
                Debug::msg("AL ERROR: OV_HOLE found in initial read of buffer {}", i);
                break;
            }
            else if (result == OV_EBADLINK)
            {
                Debug::msg("AL ERROR:  OV_EBADLINK found in initial read of buffer {}", i);
                break;
            }
            else if (result == OV_EINVAL)
            {
                Debug::msg("AL ERROR: OV_EINVAL found in initial read of buffer {}", i);
                break;
            }
            else if (result == 0)
            {
                Debug::msg("AL ERROR: EOF found in initial read of buffer {}", i);
                break;
            }

            dataSoFar += result;
        }

        if (audio_data.channels == 1 && audio_data.bitsPerSample == 8)
            audio_data.format = AL_FORMAT_MONO8;
        else if (audio_data.channels == 1 && audio_data.bitsPerSample == 16)
            audio_data.format = AL_FORMAT_MONO16;
        else if (audio_data.channels == 2 && audio_data.bitsPerSample == 8)
            audio_data.format = AL_FORMAT_STEREO8;
        else if (audio_data.channels == 2 && audio_data.bitsPerSample == 16)
            audio_data.format = AL_FORMAT_STEREO16;
        else
        {
            Debug::msg("AL ERROR: unrecognised ogg format: {} channels, {} bps", audio_data.channels, audio_data.bitsPerSample);
            delete[] data;
            return false;
        }

        alCall(alBufferData, audio_data.buffers[i], audio_data.format, data, dataSoFar, audio_data.sampleRate);
    }

    alCall(alSourceQueueBuffers, audio_data.source, NUM_BUFFERS, &audio_data.buffers[0]);

    delete[] data;

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

        char* data = new char[BUFFER_SIZE];
        std::memset(data, 0, BUFFER_SIZE);

        ALsizei dataSizeToBuffer = 0;
        std::int32_t sizeRead = 0;

        while (sizeRead < BUFFER_SIZE)
        {
            std::int32_t result = ov_read(&audio_data.ogg_vorbis_file, &data[sizeRead], BUFFER_SIZE - sizeRead, 0, 2, 1, &audio_data.ogg_current_section);
            if (result == OV_HOLE)
            {
                // std::cerr << "ERROR: OV_HOLE found in update of buffer " << std::endl;
                break;
            }
            else if (result == OV_EBADLINK)
            {
                //  std::cerr << "ERROR: OV_EBADLINK found in update of buffer " << std::endl;
                break;
            }
            else if (result == OV_EINVAL)
            {
                // std::cerr << "ERROR: OV_EINVAL found in update of buffer " << std::endl;
                break;
            }
            else if (result == 0)
            {
                std::int32_t seekResult = ov_raw_seek(&audio_data.ogg_vorbis_file, 0);

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