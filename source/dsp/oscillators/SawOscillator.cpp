#include "SawOscillator.h"

// Math: A linear ramp that climbs from -1.0 to 1.0 based on the normalized phase.
float SawOscillator::calculateSample(float currentPhase) {
    return 2.0f * currentPhase - 1.0f;
}