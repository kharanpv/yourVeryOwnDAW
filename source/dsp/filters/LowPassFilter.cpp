#include "LowPassFilter.h"
#include <cmath>
#include <algorithm>

LowPassFilter::LowPassFilter() 
    : sampleRate(44100.0f), cutoff(1000.0f), g(0.0f),
      stage1(0.0f), stage2(0.0f), stage3(0.0f), stage4(0.0f) {}

void LowPassFilter::setSampleRate(float newSampleRate) {
    sampleRate = newSampleRate;
}

void LowPassFilter::setCutoff(float cutoffHz) {
    // Keep cutoff within safe human hearing bounds
    cutoff = std::clamp(cutoffHz, 20.0f, 20000.0f);

    // Calculate the 'g' smoothing coefficient based on the sample rate.
    g = 1.0f - std::exp(-2.0f * 3.14159265359f * cutoff / sampleRate);
}

float LowPassFilter::process(float inputSample) {
    // Cascade the 4 mathematical poles (Difference Equations)
    // Formula: Output = Output + g * (Input - Output)
    
    stage1 = stage1 + g * (inputSample - stage1); // Pole 1
    stage2 = stage2 + g * (stage1 - stage2);      // Pole 2
    stage3 = stage3 + g * (stage2 - stage3);      // Pole 3
    stage4 = stage4 + g * (stage3 - stage4);      // Pole 4 (Final Smooth Output)

    return stage4;
}