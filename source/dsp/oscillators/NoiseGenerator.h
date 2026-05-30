#pragma once
#include "Oscillator.h"

class NoiseGenerator : public Oscillator {
public:
    // Noise has no pitch, so we only need to ask for amplitude.
    NoiseGenerator(float amplitude = 0.1f);

protected:
    float calculateSample(float currentPhase) override;
};