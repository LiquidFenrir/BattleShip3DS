#include "game/soundEffect.h"

#define MP3_BUF_SIZE 4096

static bool readSamples(mpg123_handle* mpg, u8* buf, size_t size)
{
    size_t readSize = 0;
    mpg123_read(mpg, buf, size, &readSize);

    DSP_FlushDataCache(buf, size);

    return readSize < size;
}

static void playThread(void* arg)
{
    SoundEffect* this_ = static_cast<SoundEffect*>(arg);
    if(this_->delay)
    {
        DEBUG("delaying %lld nanosecs\n", this_->delay);
        svcSleepThread(this_->delay);
    }
    DEBUG("Starting to play sound effect\n");
    this_->done = false;

    do
    {
        for(int i = 0; i < WAVE_BUF_AMOUNT && !this_->done && !this_->stopThread; i++)
        {
            ndspWaveBuf* waveBuf = &this_->waveBufs[i];
            if(waveBuf->status == NDSP_WBUF_DONE || waveBuf->status == NDSP_WBUF_FREE)
            {
                this_->done = readSamples(this_->mpg, (u8*)waveBuf->data_pcm16, this_->bufferSize);
                ndspChnWaveBufAdd(this_->dspChannel, waveBuf);
            }
            else
            {
                svcSleepThread(1e6);
            }
        }
    }
    while(!this_->done && !this_->stopThread);

    this_->done = true;
    DEBUG("Done playing sound\n");
}

SoundEffect::SoundEffect(const std::string& path, int dspChannel, u64 delay)
{
    DEBUG("SoundEffect::SoundEffect\n");
    if(!ndspInited)
    {
        this->valid = false;
        return;
    }

    DEBUG("attempting to load mp3 from %s at id %d\n", path.c_str(), dspChannel);

    int mpg123error = MPG123_OK;
    this->mpg = mpg123_new(NULL, &mpg123error);
    if(mpg123error != MPG123_OK)
    {
        DEBUG("mpg123_new error: %i, %s\n", mpg123error, mpg123_plain_strerror(mpg123error));
        this->valid = false;
        return;
    }

    mpg123error = mpg123_open(this->mpg, path.c_str());
    if(mpg123error != MPG123_OK)
    {
        mpg123_delete(this->mpg);
        DEBUG("mpg123_open error: %i, %s\n", mpg123error, mpg123_plain_strerror(mpg123error));
        this->valid = false;
        return;
    }

    mpg123error = mpg123_getformat(this->mpg, &this->samplerate, &this->channels, &this->encoding);
    if(mpg123error != MPG123_OK)
    {
        mpg123_close(this->mpg);
        mpg123_delete(this->mpg);
        DEBUG("mpg123_getformat error: %i, %s\n", mpg123error, mpg123_plain_strerror(mpg123error));
        this->valid = false;
        return;
    }
    else if(this->channels > 2)
    {
        mpg123_close(this->mpg);
        mpg123_delete(this->mpg);
        DEBUG("too many channels: %d\n", this->channels);
        this->valid = false;
        return;
    }

    DEBUG("channels %d, samplerate %ld\n", this->channels, this->samplerate);

    this->dspChannel = dspChannel;
    ndspChnReset(this->dspChannel);
    ndspChnWaveBufClear(this->dspChannel);
    ndspChnSetInterp(this->dspChannel, NDSP_INTERP_LINEAR);

    if(this->channels == 1)
        ndspChnSetFormat(this->dspChannel, NDSP_FORMAT_MONO_PCM16);
    else if(this->channels == 2)
        ndspChnSetFormat(this->dspChannel, NDSP_FORMAT_STEREO_PCM16);

    ndspChnSetRate(this->dspChannel, this->samplerate);
    this->bufferSize = MP3_BUF_SIZE * this->channels;
    for(int i = 0; i < WAVE_BUF_AMOUNT; i++)
    {
        ndspWaveBuf* waveBuf = &this->waveBufs[i];
        memset(waveBuf, 0, sizeof(ndspWaveBuf));

        this->audioBuffers[i] = static_cast<s16*>(linearAlloc(this->bufferSize));

        waveBuf->data_vaddr = this->audioBuffers[i];
        waveBuf->nsamples = (MP3_BUF_SIZE / this->channels);
    }

    this->valid = true;
    this->delay = delay;
    this->stopThread = false;

    s32 prio = 0;
    Result res = svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    if(R_FAILED(res) || prio <= 0x18)
        prio = 0x30;  // default priority
    this->thread = threadCreate(playThread, this, 1024, prio-1, 0, true);
}

SoundEffect::~SoundEffect()
{
    DEBUG("SoundEffect::~SoundEffect\n");
    if(this->valid)
    {
        if(this->thread)
        {
            this->stopThread = true;
            while(!this->done)
                svcSleepThread(1e6);
        }

        ndspChnWaveBufClear(this->dspChannel);

        mpg123_close(this->mpg);
        mpg123_delete(this->mpg);

        for(int i = 0; i < WAVE_BUF_AMOUNT; i++)
        {
            if(this->audioBuffers[i])
                linearFree(this->audioBuffers[i]);
        }
    }
}
