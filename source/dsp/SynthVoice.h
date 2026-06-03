#pragma once
#include "../core/IAudioSource.h"
#include "oscillators/Oscillator.h"
#include "envelopes/AhdsrEnvelope.h"
#include "filters/LowPassFilter.h"

class SynthVoice : public IAudioSource {
public:
    SynthVoice(Oscillator* osc);

    void processAudio(float* buffer, int numSamples) override;

    void triggerNote();
    void releaseNote();
    void setSampleRate(float newSampleRate);
    void setDelay(float delaySec);

    // Getters for external configuration
    AhdsrEnvelope& getAmpEnvelope();
    AhdsrEnvelope& getFilterEnvelope();
    
    // Set the base frequency of the filter, and how far the envelope pushes it up
    void setFilterParameters(float baseCutoffHz, float envDepthHz);

    // Sets the resonance (feedback) amount on the internal low pass filter
    void setFilterResonance(float resonance);

    void setOscillator(Oscillator* newOsc);

private:
    Oscillator* currentOscillator;
    LowPassFilter filter;         // The actual math block

    AhdsrEnvelope ampEnvelope;    // Controls Volume
    AhdsrEnvelope filterEnvelope; // Controls Filter Brightness
    
    float filterBaseCutoff;
    float filterEnvDepth;

    float sampleRate;
    float delaySampleCount;
    float currentDelaySample;
    bool isDelaying;
};