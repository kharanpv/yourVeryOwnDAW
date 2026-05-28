#pragma once
#include "Oscillator.h"

class TriangleOscillator : public Oscillator {
public:
    using Oscillator::Oscillator; 

protected:
    float calculateSample(float currentPhase) override;
};