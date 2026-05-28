#pragma once
#include "../../core/IAudioSource.h"

class NoiseGenerator : public IAudioSource {
public:
    NoiseGenerator(float amplitude = 0.1f);
    
    void processAudio(float* buffer, int numSamples) override;
    void setAmplitude(float newAmplitude);

private:
    float amplitude;
};