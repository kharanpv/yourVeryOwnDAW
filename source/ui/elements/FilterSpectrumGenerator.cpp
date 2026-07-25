#include "FilterSpectrumGenerator.h"

#include <cmath>

// ────────────────────────────────────────────────────────────────────────────
// FilterSpectrumGenerator::compute
//
// Models the magnitude response of a 4-pole resonant low-pass filter:
//
//   |H(f)| = |H_lp(f)| × |H_res(f)|
//
// where:
//   |H_lp(f)| = 1 / sqrt(1 + (f/fc)^8)          — 4-pole (−24 dB/oct) rolloff
//   |H_res(f)| = 1 + Q_res × (fc/f) / sqrt(1 + (f/fc)^4)  — resonant bump
//
// Q_res is derived from the normalized resonance parameter [0..1] and scaled
// so that maximum resonance produces a visible +12 dB peak at fc, matching
// typical analog subtractive synth filter behavior.
//
// The frequency axis is logarithmically spaced: f[i] = 20 × (1000)^(i / (N−1)).
// ────────────────────────────────────────────────────────────────────────────
void FilterSpectrumGenerator::compute(const Params& p, float* outBuffer) {
    static constexpr float F_MIN = 20.0f;
    static constexpr float F_MAX = 20000.0f;
    static constexpr float LOG_RATIO = 1000.0f; // F_MAX / F_MIN
    static constexpr float N_INV = 1.0f / (float)(NUM_POINTS - 1);

    float fc = p.cutoff;
    if (fc < 20.0f) fc = 20.0f;
    if (fc > 20000.0f) fc = 20000.0f;

    // Map resonance [0..1] to a Q factor [0..4] for the resonant peak.
    // At Q=4, the peak reaches roughly +12 dB (10× amplitude).
    float Q = p.resonance * 4.0f;

    for (int i = 0; i < NUM_POINTS; ++i) {
        // Logarithmically spaced frequency
        float f = F_MIN * std::pow(LOG_RATIO, (float)i * N_INV);

        // Normalized frequency ratio
        float ratio = f / fc;

        // 4-pole low-pass rolloff: |H_lp| = 1 / sqrt(1 + ratio^8)
        float ratio4 = ratio * ratio * ratio * ratio;
        float ratio8 = ratio4 * ratio4;
        float lpMag = 1.0f / std::sqrt(1.0f + ratio8);

        // Resonant bump: peaks near ratio = 1.0 (i.e., f ≈ fc)
        // |H_res| = 1 + Q × (1/ratio) / sqrt(1 + ratio^4)
        float resMag = 1.0f;
        if (ratio > 0.001f) {
            resMag = 1.0f + Q * (1.0f / ratio) / std::sqrt(1.0f + ratio4);
        }

        outBuffer[i] = lpMag * resMag;
    }
}
