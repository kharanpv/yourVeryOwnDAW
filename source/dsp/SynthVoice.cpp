#include "SynthVoice.h"

SynthVoice::SynthVoice(Oscillator* osc) 
    : currentOscillator(osc), sampleRate(44100.0f), 
      filterBaseCutoff(200.0f), filterEnvDepth(4000.0f), // Starts low, pushes up by 4000Hz
      delaySampleCount(0.0f), currentDelaySample(0.0f), isDelaying(false) {
          
    // Default Volume Envelope
    ampEnvelope.setParameters(0.01f, 0.05f, 0.1f, 0.5f, 0.2f);
    
    // Default Filter Envelope (Decays faster than volume for a plucky sweep)
    filterEnvelope.setParameters(0.01f, 0.0f, 0.15f, 0.1f, 0.2f);
}

void SynthVoice::setSampleRate(float newSampleRate) {
    sampleRate = newSampleRate;
    ampEnvelope.setSampleRate(newSampleRate);
    filterEnvelope.setSampleRate(newSampleRate);
    filter.setSampleRate(newSampleRate);
}

void SynthVoice::setFilterParameters(float baseCutoffHz, float envDepthHz) {
    filterBaseCutoff = baseCutoffHz;
    filterEnvDepth = envDepthHz;
}

void SynthVoice::setDelay(float delaySec) {
    delaySampleCount = (delaySec > 0.0f) ? (delaySec * sampleRate) : 0.0f;
}

void SynthVoice::setOscillator(Oscillator* newOsc) {
    currentOscillator = newOsc;
}

void SynthVoice::triggerNote() {
    if (delaySampleCount > 0.0f) {
        isDelaying = true;
        currentDelaySample = 0.0f;
    } else {
        isDelaying = false;
        ampEnvelope.triggerOn();
        filterEnvelope.triggerOn(); // Trigger BOTH
    }
}

void SynthVoice::releaseNote() {
    isDelaying = false;
    ampEnvelope.triggerOff();
    filterEnvelope.triggerOff(); // Release BOTH
}

AhdsrEnvelope& SynthVoice::getAmpEnvelope() { return ampEnvelope; }
AhdsrEnvelope& SynthVoice::getFilterEnvelope() { return filterEnvelope; }

void SynthVoice::processAudio(float* buffer, int numSamples) {
    if (currentOscillator == nullptr) {
        for (int i = 0; i < numSamples; ++i) buffer[i] = 0.0f;
        return;
    }

    // 1. Generate Raw Oscillator Buzz
    currentOscillator->processAudio(buffer, numSamples);

    for (int i = 0; i < numSamples; i += 2) {
        
        // Handle Pre-Delay
        if (isDelaying) {
            currentDelaySample += 1.0f;
            if (currentDelaySample >= delaySampleCount) {
                isDelaying = false;
                ampEnvelope.triggerOn();
                filterEnvelope.triggerOn();
            }
        }

        // --- SUBTRACTIVE SIGNAL CHAIN ---

        // A. Filter Envelope Calculation
        float filterEnvMultiplier = filterEnvelope.process();
        float currentCutoff = filterBaseCutoff + (filterEnvMultiplier * filterEnvDepth);
        filter.setCutoff(currentCutoff);

        // B. Filter Block (Curve the straight lines)
        // We only pass the Left channel (buffer[i]) because our synths are mono right now
        float filteredSample = filter.process(buffer[i]);

        // C. Amp Envelope Calculation
        float ampMultiplier = ampEnvelope.process();

        // D. Final Output (Filtered Sound * Volume Level)
        buffer[i] = filteredSample * ampMultiplier;       
        buffer[i + 1] = filteredSample * ampMultiplier;   
    }
}