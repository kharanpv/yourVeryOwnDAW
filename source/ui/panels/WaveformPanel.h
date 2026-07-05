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
    float height() const { return 360.0f; }

    // Render the panel. waveType: 0–4 (SINE/SAW/SQUARE/TRIANGLE/NOISE).
    void render(SharedMatrix& matrix, int waveType);

private:
    static constexpr int WAVEFORM_DISPLAY_SAMPLES = 2205; // 44100 / 20

    WaveformGenerator waveGen;
    ScopeCanvas canvas;
};