#pragma once

#include <imgui.h>
#include "../../core/SharedMatrix.h"
#include "../elements/EnvelopeGenerator.h"
#include "SignalPanel.h"

// ────────────────────────────────────────────────────────────────────────────
// EnvelopePanel
// Renders the AHDSR envelope shape.
// Inherits common canvas/background/border from SignalPanel.
// ────────────────────────────────────────────────────────────────────────────
class EnvelopePanel : public SignalPanel {
public:
    EnvelopePanel() = default;

protected:
    const char* label() const override { return "AMP ENVELOPE SHAPE"; }
    float height() const override { return 330.0f; }  // 75% of waveform's 440
    float belowGraphPadding() const override { return 40.0f; }  // x-axis labels below graph

    void drawContent(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize,
                     SharedMatrix& matrix) override;

private:
    EnvelopeGenerator envGen;
};
