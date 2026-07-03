#pragma once

#include <imgui.h>

// ────────────────────────────────────────────────────────────────────────────
// ScopeCanvas
// A reusable graph canvas with the terminal HUD aesthetic: black background,
// grey cross-hair axes, data-coordinate plotting, and label placement.
//
// Usage:
//   ScopeCanvas canvas;
//   canvas.begin(drawList, pixelPos, pixelSize, dataMinX, dataMaxX, dataMinY, dataMaxY);
//   canvas.drawAxes();
//   canvas.yLabel("1", 1.0f);
//   canvas.xLabel("25ms", 0.025f);
//   canvas.plotLine(data, count, IM_COL32_WHITE, 2.5f);
//   canvas.end();
// ────────────────────────────────────────────────────────────────────────────
class ScopeCanvas {
public:
    ScopeCanvas() = default;

    // Begin the canvas: store transform and draw the black background.
    // data bounds define the coordinate system (e.g., X=0..0.05s, Y=-1..+1).
    void begin(ImDrawList* drawList, ImVec2 pixelPos, ImVec2 pixelSize,
               float dataMinX, float dataMaxX,
               float dataMinY, float dataMaxY);

    // Draw grey cross-hair axes through center of the data Y-range.
    // The vertical axis is placed at the midpoint of the X-range.
    void drawAxes();

    // Convenience: draw both axes at once.
    void drawCrossHairs();

    // Place a text label at a data coordinate.
    void yLabel(const char* text, float dataY);
    void xLabel(const char* text, float dataX);

    // Plot a line through an array of Y values (evenly spaced in X).
    // data is sampled uniformly from dataMinX to dataMaxX.
    void plotLine(const float* yValues, int count, ImU32 color, float thickness);

    // End the canvas (reserve space so ImGui layout works).
    void end();

private:
    // Convert data-space → pixel-space
    ImVec2 dataToPixel(float dataX, float dataY) const;

    ImDrawList* dl = nullptr;
    ImVec2 pos;       // top-left pixel
    ImVec2 size;      // pixel dimensions
    float dMinX, dMaxX, dMinY, dMaxY;
    float scaleX, scaleY; // pixels per data-unit
};