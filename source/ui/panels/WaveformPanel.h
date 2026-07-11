#pragma once

#include <imgui.h>
#include "../../core/SharedMatrix.h"
#include "../elements/WaveformGenerator.h"
#include "../elements/ScopeCanvas.h"

// ────────────────────────────────────────────────────────────────────────────
// WaveformPanel
// Owns its own WaveformGenerator + ScopeCanvas.
// Reads current DSP state from SharedMatrix and renders
// a 50ms post-filter waveform preview.
// ────────────────────────────────────────────────────────────────────────────
class WaveformPanel {
public:
    // Height includes the ScopeCanvas graph + its bottom margin for x-labels
    float height() const { return 360.0f + 2.0f * 40.0f; }

    // Render the panel. waveType: 0–4 (SINE/SAW/SQUARE/TRIANGLE/NOISE).
    void render(SharedMatrix& matrix, int waveType);

private:
    static constexpr int WAVETABLE_SIZE  = 2205; // One full cycle at 2205 sampleRate
    static constexpr int PAD_SAMPLES     = 100;   // Zero-padding on each side

    WaveformGenerator waveGen;
    ScopeCanvas canvas;
};
