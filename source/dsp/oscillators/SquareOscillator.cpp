#include "SquareOscillator.h"

// Math: Acts as a binary switch. 
// If phase is under 50%, output maximum high (1.0). Otherwise, maximum low (-1.0).
float SquareOscillator::calculateSample(float currentPhase) {
    if (currentPhase < 0.5f) {
        return 1.0f;
    } else {
        return -1.0f;
    }
}