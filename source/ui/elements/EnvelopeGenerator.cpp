#include "EnvelopeGenerator.h"

void EnvelopeGenerator::generate(float* outBuffer, int numSamples,
                                  float attackSec, float holdSec,
                                  float decaySec, float sustainLevel,
                                  float releaseSec, float holdGraphicSec) const {
    if (numSamples <= 0) return;

    float totalTime = attackSec + holdSec + decaySec + holdGraphicSec + releaseSec;
    if (totalTime <= 0.0f) {
        for (int i = 0; i < numSamples; ++i) outBuffer[i] = 0.0f;
        return;
    }

    int attackSamples  = (int)((attackSec / totalTime) * numSamples);
    int holdSamples    = (int)((holdSec / totalTime) * numSamples);
    int decaySamples   = (int)((decaySec / totalTime) * numSamples);
    int graphicSamples = (int)((holdGraphicSec / totalTime) * numSamples);
    int releaseSamples = numSamples - (attackSamples + holdSamples + decaySamples + graphicSamples);

    int idx = 0;

    // Attack: 0 → 1
    for (int i = 0; i < attackSamples && idx < numSamples; ++i)
        outBuffer[idx++] = (float)i / attackSamples;

    // Hold: 1.0
    for (int i = 0; i < holdSamples && idx < numSamples; ++i)
        outBuffer[idx++] = 1.0f;

    // Decay: 1 → sustain
    for (int i = 0; i < decaySamples && idx < numSamples; ++i)
        outBuffer[idx++] = 1.0f - ((1.0f - sustainLevel) * ((float)i / decaySamples));

    // Sustain plateau (graphic hold)
    for (int i = 0; i < graphicSamples && idx < numSamples; ++i)
        outBuffer[idx++] = sustainLevel;

    // Release: sustain → 0
    for (int i = 0; i < releaseSamples && idx < numSamples; ++i)
        outBuffer[idx++] = sustainLevel * (1.0f - ((float)i / releaseSamples));

    // Fill any rounding remainder
    while (idx < numSamples) outBuffer[idx++] = 0.0f;
}