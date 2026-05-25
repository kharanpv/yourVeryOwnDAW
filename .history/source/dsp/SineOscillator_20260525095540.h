#pragma once
#include "IAudioSource.h"

class SineOscillator : public IAudioSource {
public:
    SineOscillator(float sampleRate, float frequency);
    
    void processAudio(float* buffer, int numSamples) override;
    void setFrequency(float frequency);

private:
    float sampleRate;
    float frequency;
    float phase;
    float phaseIncrement;
};