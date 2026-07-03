#include "WaveformGenerator.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int WaveformGenerator::generate(float* outBuffer, int maxSamples,
                                int waveType, float freqHz, float sampleRate,
                                float cutoffHz, float resonance,
                                float ampAttack, float ampHold, float ampDecay,
                                float ampSustain, float ampRelease) const {
    int numSamples = (maxSamples < 44100) ? maxSamples : 44100;
    float phaseIncrement = freqHz / sampleRate;

    // Envelope stage boundaries in samples
    int attackSamples = (int)(ampAttack * sampleRate);
    int holdSamples   = (int)(ampHold * sampleRate);
    int decaySamples  = (int)(ampDecay * sampleRate);

    // Clamp filter params
    if (cutoffHz < 20.0f) cutoffHz = 20.0f;
    if (cutoffHz > 20000.0f) cutoffHz = 20000.0f;
    if (resonance < 0.0f) resonance = 0.0f;
    if (resonance > 3.99f) resonance = 3.99f;
    float g = 1.0f - std::exp(-2.0f * (float)M_PI * cutoffHz / sampleRate);

    float phase = 0.0f;
    float s1 = 0.0f, s2 = 0.0f, s3 = 0.0f, s4 = 0.0f;

    for (int i = 0; i < numSamples; ++i) {
        // ── Envelope ──
        float env = 0.0f;
        if (i < attackSamples && attackSamples > 0) {
            env = (float)i / (float)attackSamples;
        } else if (i < attackSamples + holdSamples) {
            env = 1.0f;
        } else if (i < attackSamples + holdSamples + decaySamples) {
            float t = (float)(i - attackSamples - holdSamples) / (float)decaySamples;
            env = 1.0f - (1.0f - ampSustain) * t;
        } else {
            env = ampSustain;
        }

        // ── Raw oscillator ──
        float raw = 0.0f;
        switch (waveType) {
            case 0: raw = std::sin(phase * 2.0f * M_PI); break;
            case 1: raw = 2.0f * phase - 1.0f; break;
            case 2: raw = (phase < 0.5f) ? 1.0f : -1.0f; break;
            case 3: raw = 4.0f * std::abs(phase - 0.5f) - 1.0f; break;
            case 4: raw = ((float)((i * 12345 + 6789) % 1000) / 500.0f) - 1.0f; break;
            default: raw = 0.0f; break;
        }

        float enveloped = raw * env;

        // ── 4-pole cascade filter ──
        float resonatedInput = enveloped - (s4 * resonance);
        s1 = s1 + g * (resonatedInput - s1);
        s2 = s2 + g * (s1 - s2);
        s3 = s3 + g * (s2 - s3);
        s4 = s4 + g * (s3 - s4);

        float filtered = s4;
        if (filtered > 1.0f) filtered = 1.0f;
        if (filtered < -1.0f) filtered = -1.0f;

        outBuffer[i] = filtered;

        // Advance phase
        phase += phaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;
    }

    return numSamples;
}