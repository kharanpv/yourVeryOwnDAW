#pragma once

// ────────────────────────────────────────────────────────────────────────────
// WaveformGenerator
// Pure DSP — no ImGui dependency. Generates a waveform buffer by running
// the oscillator → filter → envelope chain.
//
// The output is a 50ms window (44100 samples at 44.1kHz) that can be
// decimated for display or used directly.
// ────────────────────────────────────────────────────────────────────────────
class WaveformGenerator {
public:
    // Generate a full-rate (44.1kHz) waveform into outBuffer.
    // outBuffer must be at least 44100 floats (50ms).
    // Returns the number of samples written (always 44100 for 50ms).
    int generate(float* outBuffer, int maxSamples,
                 int waveType, float freqHz, float sampleRate,
                 float cutoffHz, float resonance,
                 float ampAttack, float ampHold, float ampDecay,
                 float ampSustain, float ampRelease) const;
};