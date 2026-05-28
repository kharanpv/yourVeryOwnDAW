#pragma once
#include "Oscillator.h"

class SawOscillator : public Oscillator {
public:
    using Oscillator::Oscillator; 

protected:
    float calculateSample(float currentPhase) override;
};