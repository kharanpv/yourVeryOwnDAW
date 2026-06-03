#pragma once

// LowPassFilter (Pure 4-Pole Cascade)
// Smooths out harsh digital waves by mathematically curving sudden drops.
class LowPassFilter {
public:
    LowPassFilter();

    void setSampleRate(float newSampleRate);

    // Cutoff is the frequency (in Hz) where the filter starts smoothing.
    void setCutoff(float cutoffHz);

    // NEW: Sets the resonance feedback amount (0.0 to 4.0)
    void setResonance(float newResonance);

    // Processes a single audio sample through the 4 cascade stages
    float process(float inputSample);

private:
    float sampleRate;
    float cutoff;
    float resonance; // NEW: The feedback multiplier

    // The mathematical "smoothing" factor
    float g; 

    // The memory buffers for the 4 cascading filter stages
    float stage1;
    float stage2;
    float stage3;
    float stage4;
};