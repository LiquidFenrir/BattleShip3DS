#pragma once

#include "common.h"

#include <mpg123.h>

#define WAVE_BUF_AMOUNT 2
#define SFX_FOLDER "romfs:/sfx/"

typedef enum {
    SFX_HIT,
    SFX_MISS,
    SFX_SHOT,
    SFX_SONAR,
    
    SFX_SHIP_DEAD,
    SFX_ENEMY_SHIP_DEAD,

    SFX_AMOUNT
} SoundEffectID;

class SoundEffect
{
    public:
        SoundEffect(const std::string& path, int dspChannel, u64 delay);
        ~SoundEffect();

        bool done;
        bool stopThread;
        u64 delay;

        int dspChannel; 
        mpg123_handle* mpg;

        ndspWaveBuf waveBufs[WAVE_BUF_AMOUNT];
        size_t bufferSize;

    private:
        long int samplerate;
        int channels;
        int encoding;

        s16* audioBuffers[WAVE_BUF_AMOUNT];

        bool valid;
        Thread thread;
};
