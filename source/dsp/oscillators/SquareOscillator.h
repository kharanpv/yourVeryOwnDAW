#pragma once
#include "Oscillator.h"

class SquareOscillator : public Oscillator {
public:
    using Oscillator::Oscillator; 

protected:
    float calculateSample(float currentPhase) override;
};