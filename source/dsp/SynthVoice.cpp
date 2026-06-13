#include "SynthVoice.h"

// 1. Update Constructor to accept the Matrix
SynthVoice::SynthVoice(Oscillator* osc, std::shared_ptr<SharedMatrix> sharedMatrix) 
    : currentOscillator(osc), matrix(sharedMatrix), sampleRate(44100.0f), 
      filterBaseCutoff(1000.0f), filterEnvDepth(2000.0f), 
      delaySampleCount(0.0f), currentDelaySample(0.0f), isDelaying(false) {
          
    // Notice we removed the hardcoded envelope parameters here!
    // They will now be dynamically pulled from the Matrix in real-time.
}

void SynthVoice::setSampleRate(float newSampleRate) {
    sampleRate = newSampleRate;
    ampEnvelope.setSampleRate(newSampleRate);
    filterEnvelope.setSampleRate(newSampleRate);
    filter.setSampleRate(newSampleRate);
}

// These manual setters can stay for now, but eventually, 
// the matrix handles everything!
void SynthVoice::setFilterParameters(float baseCutoffHz, float envDepthHz) {
    filterBaseCutoff = baseCutoffHz;
    filterEnvDepth = envDepthHz;
}

void SynthVoice::setFilterResonance(float resonance) {
    filter.setResonance(resonance);
}

void SynthVoice::setDelay(float delaySec) {
    delaySampleCount = (delaySec > 0.0f) ? (delaySec * sampleRate) : 0.0f;
}

void SynthVoice::setOscillator(Oscillator* newOsc) {
    currentOscillator = newOsc;
}

void SynthVoice::triggerNote() {
    // Optionally, read the pre-delay from the matrix here when a note strikes
    float preDelaySec = matrix->ampPreDelay.load();
    delaySampleCount = (preDelaySec > 0.0f) ? (preDelaySec * sampleRate) : 0.0f;

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
    // If the Latch toggle is ON, completely ignore note off requests!
    if (matrix->isLatched.load()) return;

    isDelaying = false;
    ampEnvelope.triggerOff();
    filterEnvelope.triggerOff(); // Release BOTH
}

AhdsrEnvelope& SynthVoice::getAmpEnvelope() { return ampEnvelope; }
AhdsrEnvelope& SynthVoice::getFilterEnvelope() { return filterEnvelope; }

void SynthVoice::processAudio(float* buffer, int numSamples) {
    // Safety check: ensure both oscillator and matrix exist
    if (currentOscillator == nullptr || matrix == nullptr) {
        for (int i = 0; i < numSamples; ++i) buffer[i] = 0.0f;
        return;
    }

    // =========================================================
    // 2. THE BRIDGE: Poll the Matrix for real-time UI changes!
    // =========================================================
    
    // Update Amp Envelope
    ampEnvelope.setParameters(
        matrix->ampAttack.load(), 
        matrix->ampHold.load(), 
        matrix->ampDecay.load(), 
        matrix->ampSustain.load(), 
        matrix->ampRelease.load()
    );

    // Update Filter Envelope
    filterEnvelope.setParameters(
        matrix->filterAttack.load(), 
        matrix->filterHold.load(), 
        matrix->filterDecay.load(), 
        matrix->filterSustain.load(), 
        matrix->filterRelease.load()
    );

    // Update Filter Core
    filterBaseCutoff = matrix->filterCutoff.load();
    filterEnvDepth = matrix->filterEnvAmount.load();
    filter.setResonance(matrix->filterResonance.load());

    // =========================================================
    // 3. PROCESS THE MATH
    // =========================================================

    // Generate Raw Oscillator Buzz
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
        float filteredSample = filter.process(buffer[i]);

        // C. Amp Envelope Calculation
        float ampMultiplier = ampEnvelope.process();

        // D. Final Output (Filtered Sound * Volume Level)
        buffer[i] = filteredSample * ampMultiplier;       
        buffer[i + 1] = filteredSample * ampMultiplier;   
    }
}