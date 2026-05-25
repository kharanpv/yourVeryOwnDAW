#pragma once
#include "../core/IAudioSource.h"

// SineOscillator
// Generates a continuous mathematical sine wave. 
// Inherits from IAudioSource to interface with the AudioDevice.
class SineOscillator : public IAudioSource {
public:
    SineOscillator(float sampleRate, float frequency, float amplitude = 0.1f);
    
    void processAudio(float* buffer, int numSamples) override;
    
    // Updates the frequency of the oscillator and recalculates the phase increment.
    void setFrequency(float frequency);

    // Updates the global amplitude multiplier for the waveform.
    void setAmplitude(float newAmplitude);

private:
    float sampleRate;
    float frequency;
    float amplitude;
    
    // The current position of the waveform in its cycle (0 to 2*PI).
    float phase;
    
    // The amount to advance the phase per sample to achieve the target frequency.
    float phaseIncrement;
};