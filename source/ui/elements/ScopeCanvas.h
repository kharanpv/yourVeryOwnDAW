#pragma once

#include <imgui.h>

// ────────────────────────────────────────────────────────────────────────────
// ScopeCanvas
// A reusable graph canvas with the terminal HUD aesthetic: black background,
// bordered graph area, data-coordinate plotting, and outside-the-graph labels
// with short tick lines.
//
// Axis labels are opt-in via showYLabels / showXLabels.
//
// Usage:
//   ScopeCanvas canvas;
//   canvas.begin(drawList, pos, size, 0, 0.05f, -1, 1);
//   canvas.showYLabels(true);
//   canvas.showXLabels(false);
//   canvas.drawHorizontalAxis(0.0f);
//   canvas.yLabel("1", 1.0f);
//   canvas.plotLine(data, count, IM_COL32_WHITE, 2.5f);
//   canvas.end();
// ────────────────────────────────────────────────────────────────────────────
class ScopeCanvas {
public:
    ScopeCanvas() = default;

    void begin(ImDrawList* drawList, ImVec2 pixelPos, ImVec2 pixelSize,
               float dataMinX, float dataMaxX,
               float dataMinY, float dataMaxY);

    // Toggle label visibility (defaults: both off)
    void showYLabels(bool show) { yLabelsOn = show; }
    void showXLabels(bool show) { xLabelsOn = show; }

    void drawHorizontalAxis(float dataY);
    void drawVerticalAxis(float dataX);
    void drawCrossHairs();

    // Labels: draw text + tick line outside the graph border (no-op if disabled)
    void yLabel(const char* text, float dataY);
    void xLabel(const char* text, float dataX);

    void plotLine(const float* yValues, int count, ImU32 color, float thickness);

    void end();

private:
    ImVec2 dataToPixel(float dataX, float dataY) const;

    ImDrawList* dl = nullptr;

    // Full outer rectangle (includes margins)
    ImVec2 outerPos;
    ImVec2 outerSize;

    // Graph drawing area (inset from outer by margins on all four sides)
    ImVec2 graphPos;
    ImVec2 graphSize;

    float dMinX, dMaxX, dMinY, dMaxY;
    float scaleX, scaleY;

    bool yLabelsOn = false;
    bool xLabelsOn = false;
};
