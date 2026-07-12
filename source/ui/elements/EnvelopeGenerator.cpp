#include "EnvelopeGenerator.h"

EnvelopeNodes EnvelopeGenerator::compute(ImVec2 canvasPos, ImVec2 canvasSize,
                                         float attack,  float hold,
                                         float decay,   float sustainLevel,
                                         float release) const {
    float W = canvasSize.x;
    float H = canvasSize.y;

    // Total time of the dynamic segments (excludes the sustain plateau)
    float T_total = attack + hold + decay + release;
    if (T_total <= 0.0f) T_total = 0.001f;  // Prevent division by zero

    // Pixels per second for the 80% dynamic region
    float scale = (W * 0.8f) / T_total;

    // Sustain width is fixed at exactly 20% of canvas width
    float sustainWidth = W * 0.2f;

    // Screen-space origin
    float left   = canvasPos.x;
    float bottom = canvasPos.y + H;

    // Map amplitude [0..1] → screen Y (bottom → top)
    auto ampY = [&](float amp) { return bottom - amp * H; };

    EnvelopeNodes nodes;

    // P0: Start — bottom-left corner
    nodes.P0 = ImVec2(left, bottom);

    // P1: Peak — after Attack ramps up to amplitude 1.0
    float x1 = left + attack * scale;
    nodes.P1 = ImVec2(x1, ampY(1.0f));

    // P2: End Hold — stays at amplitude 1.0
    float x2 = x1 + hold * scale;
    nodes.P2 = ImVec2(x2, ampY(1.0f));

    // P3: Start Sustain — Decay ramps down to sustain level
    float x3 = x2 + decay * scale;
    nodes.P3 = ImVec2(x3, ampY(sustainLevel));

    // P4: End Sustain — moves right by exactly 20% of canvas width
    float x4 = x3 + sustainWidth;
    nodes.P4 = ImVec2(x4, ampY(sustainLevel));

    // P5: End Release — ramps down to zero at bottom-right
    float x5 = x4 + release * scale;
    nodes.P5 = ImVec2(x5, ampY(0.0f));

    return nodes;
}
