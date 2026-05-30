#include "SynthVoice.h"

// Constructor injects the raw oscillator and ensures the envelope is ready.
SynthVoice::SynthVoice(Oscillator* osc) : currentOscillator(osc) {
    // Set a default snappy, plucky envelope so it sounds good out of the box.
    // Attack: 0.01s, Decay: 0.1s, Sustain: 0.5 (50%), Release: 0.2s
    envelope.setParameters(0.01f, 0.1f, 0.5f, 0.2f);
}

void SynthVoice::setOscillator(Oscillator* newOsc) {
    currentOscillator = newOsc;
}

void SynthVoice::triggerNote() {
    envelope.triggerOn();
}

void SynthVoice::releaseNote() {
    envelope.triggerOff();
}

AdsrEnvelope& SynthVoice::getEnvelope() {
    return envelope;
}

// Fills the hardware buffer and shapes the volume in a single pass.
void SynthVoice::processAudio(float* buffer, int numSamples) {
    // 1. Safety check: If no oscillator is plugged in, output silence.
    if (currentOscillator == nullptr) {
        for (int i = 0; i < numSamples; ++i) {
            buffer[i] = 0.0f;
        }
        return;
    }

    // 2. Let the raw oscillator generate its continuous tone into the buffer first.
    currentOscillator->processAudio(buffer, numSamples);

    // 3. Shape that tone with the ADSR envelope.
    // We step by 2 to process Left and Right stereo channels equally.
    for (int i = 0; i < numSamples; i += 2) {
        
        // Get the volume modifier (0.0 to 1.0) for this specific moment in time.
        float envMultiplier = envelope.process();

        // Apply the envelope to both channels (in-place multiplication).
        buffer[i] *= envMultiplier;       // Left channel
        buffer[i + 1] *= envMultiplier;   // Right channel
    }
}