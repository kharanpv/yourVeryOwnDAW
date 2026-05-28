#pragma once
#include "../../core/IAudioSource.h"

// Oscillator (Base Class)
// Abstract base class handling phase accumulation, frequency math, 
// and stereo buffer processing for all pitched waveforms.
class Oscillator : public IAudioSource {
public:
    Oscillator(float sampleRate, float frequency, float amplitude = 0.1f);
    virtual ~Oscillator() = default;

    // The universal audio loop. Handles stereo interleaving and phase wrapping.
    void processAudio(float* buffer, int numSamples) override;
    
    void setFrequency(float newFrequency);
    void setAmplitude(float newAmplitude);

protected:
    // Pure virtual function. Every specific waveform MUST implement this math.
    // It takes a normalized phase (0.0 to 1.0) and returns a raw wave value.
    virtual float calculateSample(float currentPhase) = 0;

    float sampleRate;
    float frequency;
    float amplitude;
    float phase;          // Normalized: ranges from 0.0f to 1.0f
    float phaseIncrement; // How much phase to add per sample
};