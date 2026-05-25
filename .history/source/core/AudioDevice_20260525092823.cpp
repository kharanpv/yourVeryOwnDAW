#include "AudioDevice.h"
#include <iostream>

AudioDevice::AudioDevice() : deviceId(0), audioSource(nullptr) {}

AudioDevice::~AudioDevice() {
    if (deviceId != 0) {
        SDL_CloseAudioDevice(deviceId);
    }
}

bool AudioDevice::initialize(IAudioSource* source) {
    audioSource = source;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL Audio: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    
    desiredSpec.freq = 44100;          // Standard sample rate
    desiredSpec.format = AUDIO_F32SYS; // 32-bit floating point audio
    desiredSpec.channels = 2;          // Stereo output
    desiredSpec.samples = 512;         // Buffer size (latency)
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = this;       // Pass the class instance to the static callback

    deviceId = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &audioSpec, 0);
    
    if (deviceId == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void AudioDevice::start() {
    if (deviceId != 0) {
        SDL_PauseAudioDevice(deviceId, 0); // 0 unpauses the stream
    }
}

void AudioDevice::stop() {
    if (deviceId != 0) {
        SDL_PauseAudioDevice(deviceId, 1); // 1 pauses the stream
    }
}

void AudioDevice::audioCallback(void* userdata, Uint8* stream, int len) {
    AudioDevice* device = static_cast<AudioDevice*>(userdata);
    
    // SDL provides a raw byte array. We cast it to floats for DSP.
    float* buffer = reinterpret_cast<float*>(stream);
    
    // Calculate how many floats we need to process (bytes / 4).
    int numSamples = len / sizeof(float);

    if (device->audioSource) {
        device->audioSource->processAudio(buffer, numSamples);
    } else {
        // Output silence if no source is attached to prevent memory garbage noise.
        SDL_memset(stream, 0, len);
    }
}