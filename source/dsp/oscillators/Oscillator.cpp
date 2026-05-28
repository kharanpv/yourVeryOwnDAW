#include "Oscillator.h"

// Constructor initializes state. Phase always begins at 0.0f.
Oscillator::Oscillator(float sampleRate, float frequency, float amplitude)
    : sampleRate(sampleRate), amplitude(amplitude), phase(0.0f) {
    setFrequency(frequency);
}

// Calculates how much the normalized phase advances per sample.
// Formula: Δϕ = f / fs
void Oscillator::setFrequency(float newFrequency) {
    frequency = newFrequency;
    phaseIncrement = frequency / sampleRate;
}

void Oscillator::setAmplitude(float newAmplitude) {
    amplitude = newAmplitude;
}

// The master audio callback. 
void Oscillator::processAudio(float* buffer, int numSamples) {
    // Step by 2 for Left/Right interleaved stereo
    for (int i = 0; i < numSamples; i += 2) {
        
        // 1. Ask the child class to generate the raw wave shape at the current phase
        // 2. Multiply by amplitude for volume control
        float sample = calculateSample(phase) * amplitude; 
        
        // Write to both channels (dual mono)
        buffer[i]     = sample; 
        buffer[i + 1] = sample; 
        
        // Advance the normalized phase
        phase += phaseIncrement;
        
        // Wrap phase back to 0.0f once it hits 1.0f (100% of a cycle)
        if (phase >= 1.0f) {
            phase -= 1.0f; 
        }
    }
}