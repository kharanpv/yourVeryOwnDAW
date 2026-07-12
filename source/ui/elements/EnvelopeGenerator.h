#pragma once

#include <imgui.h>

// ────────────────────────────────────────────────────────────────────────────
// EnvelopeNodes
// The 6 anchor points of an AHDSR envelope in screen-space ImGui coordinates.
// ────────────────────────────────────────────────────────────────────────────
struct EnvelopeNodes {
    ImVec2 P0;  // Start (bottom-left)
    ImVec2 P1;  // Peak (top, amplitude 1.0)
    ImVec2 P2;  // End hold (top, amplitude 1.0)
    ImVec2 P3;  // Start sustain (drops to sustain level)
    ImVec2 P4;  // End sustain (moved right by 20% of canvas width)
    ImVec2 P5;  // End release (bottom-right corner)
};

// ────────────────────────────────────────────────────────────────────────────
// EnvelopeGenerator
// Computes the 6 anchor nodes (P0–P5) of an AHDSR envelope as screen-space
// ImGui coordinates for direct polyline rendering.
//
// Layout rules (from the UI Rendering Blueprint):
//   - Y-axis: fixed [0.0 → 1.0] mapped to canvas bottom → top.
//   - X-axis: Sustain phase is hardcoded to 20% of canvas width.
//              The remaining 80% is distributed proportionally among
//              Attack + Hold + Decay + Release (actual time values).
// ────────────────────────────────────────────────────────────────────────────
class EnvelopeGenerator {
public:
    // Compute the 6 polyline nodes in pixel space.
    //
    //   canvasPos       – top-left screen position of the canvas region
    //   canvasSize      – full width × height of the canvas region (pixels)
    //   attack, hold, decay, release – time in seconds
    //   sustainLevel    – amplitude 0.0 → 1.0
    EnvelopeNodes compute(ImVec2 canvasPos, ImVec2 canvasSize,
                          float attack,  float hold,
                          float decay,   float sustainLevel,
                          float release) const;
};
