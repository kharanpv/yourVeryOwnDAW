#pragma once

#include <imgui.h>
#include "../../core/SharedMatrix.h"

// ────────────────────────────────────────────────────────────────────────────
// SignalPanel
// Abstract base for the right-column signal panels (Waveform, Envelope).
// Enforces identical canvas dimensions, background, border, and layout.
//
// Subclasses implement drawContent() with only the graph-specific rendering.
// ────────────────────────────────────────────────────────────────────────────
class SignalPanel {
public:
    virtual ~SignalPanel() = default;

    // Called by the parent (SynthDashboard). Handles the common frame,
    // then delegates to the subclass for graph-specific drawing.
    void render(SharedMatrix& matrix);

protected:
    // Subclasses return their label (e.g., "WAVEFORM", "AMP ENVELOPE SHAPE").
    virtual const char* label() const = 0;

    // Subclasses return their fixed panel height in pixels.
    virtual float height() const = 0;

    // Extra pixels needed below the graph border (e.g., for x-axis labels).
    // Default: 0 (no extra space). Override if your graph has below-border content.
    virtual float belowGraphPadding() const { return 0.0f; }

    // Subclasses draw their graph content into the provided canvas region.
    //   dl         - the active ImDrawList
    //   canvasPos  - top-left of the graph region (inside the margin)
    //   canvasSize - width x height of the graph region
    virtual void drawContent(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize,
                             SharedMatrix& matrix) = 0;

private:
    // Common constants (matching ScopeCanvas's labelMargin)
    static constexpr float LABEL_MARGIN = 40.0f;

    void drawBackground(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize);
    void drawBorder(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize);
};
