#pragma once

// ────────────────────────────────────────────────────────────────────────────
// EnvelopeGenerator
// Pure math — no ImGui dependency. Generates an AHDSR envelope shape.
// Output is an array of values in [0, 1] representing the envelope contour.
// ────────────────────────────────────────────────────────────────────────────
class EnvelopeGenerator {
public:
    // Generate an AHDSR envelope into outBuffer.
    // numSamples: how many points to generate (e.g., 100 for a plot).
    // The envelope assumes a note-on at time 0 and a note-off after
    // the sustain plateau (release is approximated via a fixed hold).
    void generate(float* outBuffer, int numSamples,
                  float attackSec, float holdSec,
                  float decaySec, float sustainLevel,
                  float releaseSec, float holdGraphicSec = 0.1f) const;
};