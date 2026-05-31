#pragma once
#include "../core/IAudioSource.h"
#include "oscillators/Oscillator.h"
#include "envelopes/AhdsrEnvelope.h"

// SynthVoice
// A self-contained instrument engine that pairs a raw waveform generator (Oscillator)
// with a dynamic volume shaper (AhdsrEnvelope), including an initial start delay.
class SynthVoice : public IAudioSource {
public:
    SynthVoice(Oscillator* osc);

    void processAudio(float* buffer, int numSamples) override;

    void triggerNote();
    void releaseNote();

    // Required to calculate accurate delays
    void setSampleRate(float newSampleRate);

    // Set the amount of time (in seconds) before the attack phase begins
    void setDelay(float delaySec);

    AhdsrEnvelope& getEnvelope();
    void setOscillator(Oscillator* newOsc);

private:
    Oscillator* currentOscillator;
    AhdsrEnvelope envelope;
    
    // Pre-Delay Timer Variables
    float sampleRate;
    float delaySampleCount;
    float currentDelaySample;
    bool isDelaying;
};