#pragma once
#include <SDL2/SDL.h>
#include "IAudioSource.h"

class AudioDevice {
public:
    AudioDevice();
    ~AudioDevice();

    bool initialize(IAudioSource* source);
    void start();
    void stop();

private:
    // SDL requires a static function for its C-style callback mechanism.
    static void audioCallback(void* userdata, Uint8* stream, int len);
    
    SDL_AudioDeviceID deviceId;
    SDL_AudioSpec audioSpec;
    IAudioSource* audioSource;
};