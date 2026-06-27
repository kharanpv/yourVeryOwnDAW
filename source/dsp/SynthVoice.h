#pragma once
#include <memory> // For std::shared_ptr
#include "../core/IAudioSource.h"
#include "../core/SharedMatrix.h" 
#include "oscillators/Oscillator.h"
#include "envelopes/AhdsrEnvelope.h"
#include "filters/LowPassFilter.h"

class SynthVoice : public IAudioSource {
public:
    SynthVoice(Oscillator* osc, std::shared_ptr<SharedMatrix> matrix, int trackId);

    void processAudio(float* buffer, int numSamples) override;

    void triggerNote();
    void releaseNote();
    // Tunes the oscillator to the given MIDI note number, then fires a note-on.
    // midiNote: standard MIDI note number (e.g. 60 = C4, 69 = A4)
    void playNote(int midiNote);
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
    std::shared_ptr<SharedMatrix> matrix; // THE BRIDGE
    int trackId;

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