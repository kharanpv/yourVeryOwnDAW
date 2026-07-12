#pragma once

#include <imgui.h>
#include "../../core/SharedMatrix.h"
#include "../elements/WaveformGenerator.h"
#include "SignalPanel.h"

// ────────────────────────────────────────────────────────────────────────────
// WaveformPanel
// Renders a single-cycle waveform preview.
// Inherits common canvas/background/border from SignalPanel.
// ────────────────────────────────────────────────────────────────────────────
class WaveformPanel : public SignalPanel {
public:
    WaveformPanel() = default;

protected:
    const char* label() const override { return "WAVEFORM"; }
    float height() const override { return 330.0f; }
    float belowGraphPadding() const override { return 40.0f; }  // x-axis labels below graph

    void drawContent(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize,
                     SharedMatrix& matrix) override;

private:
    static constexpr int WAVETABLE_SIZE  = 2205;
    static constexpr int PAD_SAMPLES     = 100;

    WaveformGenerator waveGen;
};
