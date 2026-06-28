#include "SynthVoice.h"
#include <cmath>

// 1. Constructor to accept the Matrix
SynthVoice::SynthVoice(Oscillator* osc, std::shared_ptr<SharedMatrix> sharedMatrix, int trackId) 
    : currentOscillator(osc), matrix(sharedMatrix), trackId(trackId), sampleRate(44100.0f), 
      filterBaseCutoff(1000.0f), filterEnvDepth(2000.0f), 
      delaySampleCount(0.0f), currentDelaySample(0.0f), isDelaying(false) {}

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
    float preDelaySec = matrix->tracks[trackId].params[P_AMP_PREDELAY].load();
    delaySampleCount = (preDelaySec > 0.0f) ? (preDelaySec * sampleRate) : 0.0f;

    if (delaySampleCount > 0.0f) {
        isDelaying = true;
        currentDelaySample = 0.0f;
    } else {
        isDelaying = false;
        ampEnvelope.triggerOn();
        filterEnvelope.triggerOn(); 
    }
}

void SynthVoice::releaseNote() {
    // Read latch mode as a float (> 0.5 is ON)
    if (matrix->tracks[trackId].params[P_LATCH_MODE].load() > 0.5f) return;

    isDelaying = false;
    ampEnvelope.triggerOff();
    filterEnvelope.triggerOff(); 
}

AhdsrEnvelope& SynthVoice::getAmpEnvelope() { return ampEnvelope; }
AhdsrEnvelope& SynthVoice::getFilterEnvelope() { return filterEnvelope; }

void SynthVoice::playNote(int midiNote) {
    float freq = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
    currentOscillator->setFrequency(freq);
    triggerNote();
}

void SynthVoice::processAudio(float* buffer, int numSamples) {
    // Safety check: ensure both oscillator and matrix exist
    if (currentOscillator == nullptr || matrix == nullptr) {
        for (int i = 0; i < numSamples; ++i) buffer[i] = 0.0f;
        return;
    }

    // =========================================================
    // 2. THE BRIDGE: Poll the Matrix for real-time UI changes!
    // =========================================================
    
    ampEnvelope.setParameters(
        matrix->tracks[trackId].params[P_AMP_ATTACK].load(), 
        matrix->tracks[trackId].params[P_AMP_HOLD].load(), 
        matrix->tracks[trackId].params[P_AMP_DECAY].load(), 
        matrix->tracks[trackId].params[P_AMP_SUSTAIN].load(), 
        matrix->tracks[trackId].params[P_AMP_RELEASE].load()
    );

    filterEnvelope.setParameters(
        matrix->tracks[trackId].params[P_FILTER_ATTACK].load(), 
        matrix->tracks[trackId].params[P_FILTER_HOLD].load(), 
        matrix->tracks[trackId].params[P_FILTER_DECAY].load(), 
        matrix->tracks[trackId].params[P_FILTER_SUSTAIN].load(), 
        matrix->tracks[trackId].params[P_FILTER_RELEASE].load()
    );

    filterBaseCutoff = matrix->tracks[trackId].params[P_FILTER_CUTOFF].load();
    filterEnvDepth = matrix->tracks[trackId].params[P_FILTER_ENV_AMT].load();
    filter.setResonance(matrix->tracks[trackId].params[P_FILTER_RES].load());

    // =========================================================
    // 3. PROCESS THE MATH
    // =========================================================

    const float volume = matrix->masterVolume.load(std::memory_order_relaxed);

    // Generate Raw Oscillator Buzz
    currentOscillator->processAudio(buffer, numSamples);

    // Reset scope write index so each audio callback writes a fresh frame
    matrix->scopeWriteIndex.store(0, std::memory_order_relaxed);

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
        buffer[i] = filteredSample * ampMultiplier * volume;
        buffer[i + 1] = filteredSample * ampMultiplier * volume;

        // =========================================================
        // 4. WRITE TO THE OSCILLOSCOPE BUFFER
        //    The filtered sample (post-filter) shows how cutoff/res
        //    affect the waveform visually.
        // =========================================================
        int scopeIdx = matrix->scopeWriteIndex.load(std::memory_order_relaxed);
        if (scopeIdx < SharedMatrix::SCOPE_SIZE) {
            matrix->oscilloscopeBuffer[scopeIdx].store(buffer[i], std::memory_order_relaxed);
            matrix->scopeWriteIndex.store(scopeIdx + 1, std::memory_order_relaxed);
        }
    }
}