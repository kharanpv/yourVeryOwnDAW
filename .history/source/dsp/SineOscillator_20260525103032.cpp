// SineOscillator.cpp
// Implementation of a rudimentary digital sine wave generator.
// Calculates the phase increment necessary to generate a specific frequency
// at a given sample rate, and outputs the resulting waveform interleaved for stereo.

#include "SineOscillator.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SineOscillator::SineOscillator(float sampleRate, float frequency, float amplitude)
    : sampleRate(sampleRate), frequency(frequency), amplitude(amplitude), phase(0.0f) {
    setFrequency(frequency);
}

// Recalculates the phase increment. 
// Formula: Δϕ = (2 * π * f) / fs
// This determines how far along the sine curve we move per individual sample.
void SineOscillator::setFrequency(float newFrequency) {
    frequency = newFrequency;
    phaseIncrement = (2.0f * M_PI * frequency) / sampleRate;
}

// Updates the amplitude scalar. 
// Expected range is 0.0f (silence) to 1.0f (maximum digital volume).
void SineOscillator::setAmplitude(float newAmplitude) {
    amplitude = newAmplitude;
}

// The audio callback execution. Fills the hardware buffer with sine wave data.
// - buffer: Pointer to the float array provided by the audio hardware.
// - numSamples: Total number of floats to write. For stereo, this is frames * 2.
void SineOscillator::processAudio(float* buffer, int numSamples) {
    // Iterate through the buffer. Step by 2 to handle interleaved Left/Right channels.
    for (int i = 0; i < numSamples; i += 2) {
        
        // Calculate the current sample value using the mathematical sine function.
        // Multiply by the amplitude scalar to govern output volume.
        float sample = std::sin(phase) * amplitude; 
        
        // Write the exact same sample to both the left and right channels (dual mono).
        buffer[i]     = sample; // Left channel
        buffer[i + 1] = sample; // Right channel
        
        // Advance the phase for the next sample calculation.
        phase += phaseIncrement;
        
        // Wrap the phase back to 0 once it completes a full cycle (2*PI).
        // This prevents floating-point overflow during continuous, long-term playback.
        if (phase >= 2.0f * M_PI) {
            phase -= 2.0f * M_PI; 
        }
    }
}