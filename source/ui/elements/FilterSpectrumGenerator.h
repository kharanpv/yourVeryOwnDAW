#pragma once

// ────────────────────────────────────────────────────────────────────────────
// FilterSpectrumGenerator
// Pure DSP utility that computes the magnitude response of the 4-pole
// resonant low-pass filter across a logarithmically spaced frequency axis.
//
// No ImGui, no SharedMatrix, no audio thread — just math in, float array out.
//
// The transfer function models a cascaded 4-pole (−24 dB/oct) low-pass
// filter with a single resonant feedback path at the cutoff frequency.
// ────────────────────────────────────────────────────────────────────────────
class FilterSpectrumGenerator {
public:
    static constexpr int NUM_POINTS = 2205;

    struct Params {
        float cutoff;    // Hz, range [20 .. 20000]
        float resonance; // normalized [0.0 .. 1.0]
    };

    // Fills `outBuffer` with `NUM_POINTS` amplitude multipliers (linear scale).
    // The frequency axis is logarithmically spaced from 20 Hz to 20,000 Hz.
    static void compute(const Params& p, float* outBuffer);
};
