#include "SineOscillator.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Generates the sine value. 
// Since our base phase is normalized (0.0 to 1.0), we must multiply 
// it by 2*PI here to satisfy the standard std::sin() function.
float SineOscillator::calculateSample(float currentPhase) {
    return std::sin(currentPhase * 2.0f * M_PI);
}