#pragma once

#include <imgui.h>
#include "../../core/SharedMatrix.h"
#include "../elements/EnvelopeGenerator.h"
#include "../elements/ScopeCanvas.h"

// ────────────────────────────────────────────────────────────────────────────
// EnvelopePanel
// Owns its own EnvelopeGenerator + ScopeCanvas.
// Reads AHDSR parameters from SharedMatrix and renders the amp envelope shape.
// ────────────────────────────────────────────────────────────────────────────
class EnvelopePanel {
public:
    // Height includes the ScopeCanvas graph + its bottom margin for x-labels
    float height() const { return 252.0f + 2.0f * 40.0f; }

    // Render the envelope shape panel.
    void render(SharedMatrix& matrix);

private:
    EnvelopeGenerator envGen;
    ScopeCanvas canvas;
};
