#include "NoiseGenerator.h"
#include <cstdlib> // Required for rand() and RAND_MAX

// Constructor passes dummy values (44100.0f sample rate, 0.0f frequency) 
// up to the base Oscillator class since white noise does not use them.
NoiseGenerator::NoiseGenerator(float amplitude) 
    : Oscillator(44100.0f, 0.0f, amplitude) {}

float NoiseGenerator::calculateSample(float currentPhase) {
    // White noise does not follow a repeating wave cycle.
    // We completely ignore the 'currentPhase' argument and simply 
    // return a random float between -1.0f and 1.0f.
    
    float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return (randomValue * 2.0f) - 1.0f;
}