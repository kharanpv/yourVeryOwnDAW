#pragma once

#include <imgui.h>
#include "../../core/SharedMatrix.h"
#include "../elements/FilterSpectrumGenerator.h"
#include "SignalPanel.h"

// ────────────────────────────────────────────────────────────────────────────
// FilterSpectrumPanel
// Renders the real-time frequency-domain transfer function of the 4-pole
// resonant low-pass filter. Reads Cutoff and Resonance atomics from the
// SharedMatrix and computes 2205-point magnitude response each frame.
//
// Inherits common canvas/background/border from SignalPanel.
// Placed below the EnvelopePanel in the right Telemetry column.
// ────────────────────────────────────────────────────────────────────────────
class FilterSpectrumPanel : public SignalPanel {
public:
    FilterSpectrumPanel() = default;

protected:
    const char* label() const override { return "FILTER SPECTRUM"; }
    float height() const override { return 330.0f; }
    float belowGraphPadding() const override { return 40.0f; }

    void drawContent(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize,
                     SharedMatrix& matrix) override;

private:
    // Navy blue for the filter transfer function line
    static constexpr ImU32 COL_FILTER_LINE = IM_COL32(65, 105, 225, 255);  // Royal blue
};
