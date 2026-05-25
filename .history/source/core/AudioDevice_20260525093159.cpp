// AudioDevice.cpp
// Small wrapper around SDL audio device to connect an IAudioSource to
// the system audio output. This file manages device lifecycle, starts/stops
// the stream, and forwards audio requests from SDL to the attached source.

#include "AudioDevice.h"
#include <iostream>

//`deviceId` of 0 means "no device opened".
AudioDevice::AudioDevice() : deviceId(0), audioSource(nullptr) {}

// Destructor: close the SDL audio device if it was opened.
AudioDevice::~AudioDevice() {
    if (deviceId != 0) {
        // SDL_CloseAudioDevice will release the OS audio handle.
        SDL_CloseAudioDevice(deviceId);
    }
}

// Initialize the audio subsystem and open a device for playback.
// - `source` is an object implementing `IAudioSource` which will provide
//   audio data when SDL requests it via the callback.
// Returns true on success, false on failure.
bool AudioDevice::initialize(IAudioSource* source) {
    audioSource = source;

    // Make sure the SDL audio subsystem is initialized. This only initializes
    // the audio part of SDL (SDL_Init(SDL_INIT_AUDIO) is more heavy-weight).
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL Audio: " << SDL_GetError() << std::endl;
        return false;
    }

    // Prepare the desired audio format we want from the device.
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec); // zero the struct for safety
    
    // Common settings for a DAW-like playback: 44.1kHz, 32-bit float, stereo.
    desiredSpec.freq = 44100;          // Sample rate in Hz
    desiredSpec.format = AUDIO_F32SYS; // 32-bit floating point samples in native endian
    desiredSpec.channels = 2;          // Stereo (left + right)
    desiredSpec.samples = 512;         // Size of the audio buffer in samples (affects latency)
    desiredSpec.callback = audioCallback; // Static C callback called by SDL in audio thread
    desiredSpec.userdata = this;       // Pass `this` so the static callback can access instance data

    // Open the default audio device for output (second parameter 0 means playback).
    // `audioSpec` is filled with the actual format provided by the device.
    deviceId = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &audioSpec, 0);
    
    if (deviceId == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

// Start audio playback by unpausing the device. SDL runs the callback while
// the device is unpaused and pulls audio data into the output buffer.
void AudioDevice::start() {
    if (deviceId != 0) {
        SDL_PauseAudioDevice(deviceId, 0); // 0 = unpause (start streaming)
    }
}

// Pause audio playback. Useful for stopping sound without closing the device.
void AudioDevice::stop() {
    if (deviceId != 0) {
        SDL_PauseAudioDevice(deviceId, 1); // 1 = pause (stop streaming)
    }
}

// Static callback called by SDL on the audio thread. It receives a pointer to
// raw bytes (`Uint8* stream`) and the length in bytes (`len`). We reinterpret
// that buffer as floats (because we requested AUDIO_F32SYS) and forward the
// request to the attached `IAudioSource`.
void AudioDevice::audioCallback(void* userdata, Uint8* stream, int len) {
    // Recover the instance pointer that we stored in `desiredSpec.userdata`.
    AudioDevice* device = static_cast<AudioDevice*>(userdata);
    
    // SDL provides a raw byte array. Cast to float* because the chosen
    // audio format is 32-bit float (AUDIO_F32SYS).
    float* buffer = reinterpret_cast<float*>(stream);
    
    // Number of float samples to process: bytes divided by size of float.
    int numSamples = len / sizeof(float);

    if (device->audioSource) {
        // Let the audio source fill the buffer. The source must be thread-safe
        // and real-time safe because this runs on the audio thread.
        device->audioSource->processAudio(buffer, numSamples);
    } else {
        // If no source is attached, write silence to the output buffer to avoid noise.
        SDL_memset(stream, 0, len);
    }
}