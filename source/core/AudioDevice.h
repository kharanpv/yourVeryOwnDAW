#pragma once
#include <SDL2/SDL.h>
#include "IAudioSource.h"

// AudioDevice
// Brief: Manages an SDL audio device and forwards audio data from an `IAudioSource`
// to SDL's audio subsystem.
//
// Ownership: `initialize` takes a non-owning pointer to `IAudioSource`. The
// caller is responsible for ensuring the source remains valid for the
// lifetime of the `AudioDevice` (or until reinitialization).
class AudioDevice {
public:
    // Construct / destruct: destructor stops playback and cleans up the SDL
    // device if it was opened.
    AudioDevice();
    ~AudioDevice();

    // initialize: open and configure the SDL audio device. Returns true on
    // success. `source` is stored as a non-owning pointer and will be used by
    // the audio callback to pull sample data.
    bool initialize(IAudioSource* source);

    // start / stop: control audio output. `start()` unpauses the device so
    // SDL begins calling the audio callback; `stop()` pauses playback.
    void start();
    void stop();

private:
    // This static wrapper forwards to the instance (via userdata) to fill `stream` with `len`
    // bytes of audio from `audioSource`.
    static void audioCallback(void* userdata, Uint8* stream, int len);
    
    SDL_AudioDeviceID deviceId;

    SDL_AudioSpec audioSpec;

    IAudioSource* audioSource;
};