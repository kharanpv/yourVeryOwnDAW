#pragma once

class IAudioSource {
public:
    virtual ~IAudioSource() = default;
    
    // The core DSP callback. 
    // buffer: A pointer to the audio hardware's float array.
    // numSamples: The total number of floats to write (frames * channels).
    virtual void processAudio(float* buffer, int numSamples) = 0;
};