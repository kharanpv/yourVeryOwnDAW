#include "SynthVoice.h"

// Constructor injects the raw oscillator and ensures the engine is ready.
SynthVoice::SynthVoice(Oscillator* osc) 
    : currentOscillator(osc), sampleRate(44100.0f), 
      delaySampleCount(0.0f), currentDelaySample(0.0f), isDelaying(false) {
          
    // Default snappy, punchy AHDSR envelope
    // Attack: 0.01s, Hold: 0.05s, Decay: 0.1s, Sustain: 0.5 (50%), Release: 0.2s
    envelope.setParameters(0.01f, 0.05f, 0.1f, 0.5f, 0.2f);
}

void SynthVoice::setSampleRate(float newSampleRate) {
    sampleRate = newSampleRate;
    envelope.setSampleRate(newSampleRate);
}

void SynthVoice::setDelay(float delaySec) {
    // Convert seconds to samples
    delaySampleCount = (delaySec > 0.0f) ? (delaySec * sampleRate) : 0.0f;
}

void SynthVoice::setOscillator(Oscillator* newOsc) {
    currentOscillator = newOsc;
}

void SynthVoice::triggerNote() {
    if (delaySampleCount > 0.0f) {
        // Start the silent delay phase
        isDelaying = true;
        currentDelaySample = 0.0f;
    } else {
        // Skip delay, trigger envelope immediately
        isDelaying = false;
        envelope.triggerOn();
    }
}

void SynthVoice::releaseNote() {
    isDelaying = false; // Cancel delay if the key is released early
    envelope.triggerOff();
}

AhdsrEnvelope& SynthVoice::getEnvelope() {
    return envelope;
}

// Fills the hardware buffer and shapes the volume in a single pass.
void SynthVoice::processAudio(float* buffer, int numSamples) {
    if (currentOscillator == nullptr) {
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = 0.0f;
        }
        return;
    }

    // 1. Let the raw oscillator generate its continuous tone into the buffer.
    currentOscillator->processAudio(buffer, numSamples);

    // 2. Shape that tone with the Pre-Delay and AHDSR envelope.
    for (int i = 0; i < numSamples; i += 2) {
        
        // Handle the Delay Timer
        if (isDelaying) {
            currentDelaySample += 1.0f;
            if (currentDelaySample >= delaySampleCount) {
                isDelaying = false;
                envelope.triggerOn();
            }
        }

        // Get the volume modifier (0.0 to 1.0)
        float envMultiplier = envelope.process();

        // Apply the envelope to both channels
        buffer[i] *= envMultiplier;       // Left channel
        buffer[i + 1] *= envMultiplier;   // Right channel
    }
}