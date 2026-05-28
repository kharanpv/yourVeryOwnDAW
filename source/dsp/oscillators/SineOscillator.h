#pragma once
#include "Oscillator.h"

// SineOscillator
// Generates a pure mathematical sine wave.
class SineOscillator : public Oscillator {
public:
    // Inherit the base constructor directly
    using Oscillator::Oscillator; 

protected:
    // Implement the specific math for a sine wave
    float calculateSample(float currentPhase) override;
};