#include "TriangleOscillator.h"
#include <cmath>

// Math: A linear ramp up followed by a linear ramp down.
float TriangleOscillator::calculateSample(float currentPhase) {
    return 4.0f * std::abs(currentPhase - 0.5f) - 1.0f;
}
