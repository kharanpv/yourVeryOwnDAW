#include "SineOscillator.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SineOscillator::SineOscillator(float sampleRate, float frequency)
    : sampleRate(sampleRate), frequency(frequency), phase(0.0f) {
    setFrequency(frequency);
}

void SineOscillator::setFrequency(float newFrequency) {
    frequency = newFrequency;
    // Calculate how much to advance the sine wave every sample
    phaseIncrement = (2.0f * M_PI * frequency) / sampleRate;
}

void SineOscillator::processAudio(float* buffer, int numSamples) {
    // Process frames. Step by 2 because the buffer is interleaved Left/Right stereo.
    for (int i = 0; i < numSamples; i += 2) {
        // Multiply by 0.1f to lower the volume. Pure sine waves are loud.
        float sample = std::sin(phase) * 0.1f; 
        
        buffer[i]     = sample; // Left channel
        buffer[i + 1] = sample; // Right channel
        
        phase += phaseIncrement;
        if (phase >= 2.0f * M_PI) {
            phase -= 2.0f * M_PI; // Keep phase within bounds
        }
    }
}