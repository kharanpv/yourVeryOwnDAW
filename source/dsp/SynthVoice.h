#pragma once
#include "../core/IAudioSource.h"
#include "oscillators/Oscillator.h"
#include "envelopes/AdsrEnvelope.h"

// SynthVoice
// A self-contained instrument engine that pairs a raw waveform generator (Oscillator)
// with a dynamic volume shaper (AdsrEnvelope). 
class SynthVoice : public IAudioSource {
public:
    // Takes a pointer to any valid Oscillator (Sine, Saw, Square, etc.)
    SynthVoice(Oscillator* osc);

    // The universal contract function requested by AudioDevice.
    void processAudio(float* buffer, int numSamples) override;

    // Hardware interaction points.
    void triggerNote();
    void releaseNote();

    // Allows external configuration of the envelope.
    AdsrEnvelope& getEnvelope();
    
    // Allows swapping the oscillator on the fly.
    void setOscillator(Oscillator* newOsc);

private:
    Oscillator* currentOscillator;
    AdsrEnvelope envelope;
};