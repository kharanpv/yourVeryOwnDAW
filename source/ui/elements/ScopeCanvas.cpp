#include "ScopeCanvas.h"
#include "TerminalStyle.h"
#include <cmath>

void ScopeCanvas::begin(ImDrawList* drawList, ImVec2 pixelPos, ImVec2 pixelSize,
                        float dataMinX, float dataMaxX,
                        float dataMinY, float dataMaxY) {
    dl = drawList;
    outerPos  = pixelPos;
    outerSize = pixelSize;
    dMinX = dataMinX;
    dMaxX = dataMaxX;
    dMinY = dataMinY;
    dMaxY = dataMaxY;

    // Graph area is inset from the outer rect by the uniform label margin
    float m = TerminalStyle::labelMargin();
    graphPos  = ImVec2(outerPos.x + m, outerPos.y + m);
    graphSize = ImVec2(outerSize.x - 2.0f * m, outerSize.y - 2.0f * m);

    float dx = dMaxX - dMinX;
    float dy = dMaxY - dMinY;
    scaleX = (dx != 0.0f) ? graphSize.x / dx : 1.0f;
    scaleY = (dy != 0.0f) ? graphSize.y / dy : 1.0f;

    // Black background over the full region
    dl->AddRectFilled(outerPos,
                      ImVec2(outerPos.x + outerSize.x, outerPos.y + outerSize.y),
                      TerminalStyle::colBlack());

    // Border around the graph area
    dl->AddRect(graphPos,
                ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y),
                TerminalStyle::colBorder(), 0.0f, 0, TerminalStyle::axisThickness());
}

void ScopeCanvas::drawHorizontalAxis(float dataY) {
    ImVec2 left  = dataToPixel(dMinX, dataY);
    ImVec2 right = dataToPixel(dMaxX, dataY);
    dl->AddLine(left, right,
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());
}

void ScopeCanvas::drawVerticalAxis(float dataX) {
    ImVec2 top = dataToPixel(dataX, dMaxY);
    ImVec2 bot = dataToPixel(dataX, dMinY);
    dl->AddLine(top, bot,
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());
}

void ScopeCanvas::drawCrossHairs() {
    drawHorizontalAxis((dMinY + dMaxY) * 0.5f);
    drawVerticalAxis((dMinX + dMaxX) * 0.5f);
}

void ScopeCanvas::yLabel(const char* text, float dataY) {
    if (!yLabelsOn) return;

    ImVec2 p = dataToPixel(dMinX, dataY);

    // Tick: inner edge of border → inward into margin
    float tick = TerminalStyle::tickLength();
    float half = TerminalStyle::axisThickness() * 0.5f;
    dl->AddLine(ImVec2(graphPos.x - half, p.y),
                ImVec2(graphPos.x - half - tick, p.y),
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());

    // Text: right-aligned, sitting above the tick point
    float textW = ImGui::CalcTextSize(text).x;
    float tx = graphPos.x - half - tick - 4.0f - textW;
    float ty = p.y - ImGui::GetTextLineHeight();
    dl->AddText(ImVec2(tx, ty), TerminalStyle::colDimText(), text);
}

void ScopeCanvas::xLabel(const char* text, float dataX) {
    if (!xLabelsOn) return;

    ImVec2 p = dataToPixel(dataX, dMinY);

    // Tick: inner edge of border → downward into margin
    float tick = TerminalStyle::tickLength();
    float half = TerminalStyle::axisThickness() * 0.5f;
    float graphBottom = graphPos.y + graphSize.y + half;
    dl->AddLine(ImVec2(p.x, graphBottom),
                ImVec2(p.x, graphBottom + tick),
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());

    // Text: horizontally centered, below the tick
    float textW = ImGui::CalcTextSize(text).x;
    float tx = p.x - textW * 0.5f;
    float ty = graphBottom + tick + 2.0f;

    // Clamp to outer rect bounds
    if (tx < outerPos.x) tx = outerPos.x;
    if (tx + textW > outerPos.x + outerSize.x)
        tx = outerPos.x + outerSize.x - textW;

    dl->AddText(ImVec2(tx, ty), TerminalStyle::colDimText(), text);
}

void ScopeCanvas::plotLine(const float* yValues, int count, ImU32 color, float thickness) {
    if (count < 2) return;
    float stepX = (dMaxX - dMinX) / (float)(count - 1);
    for (int i = 1; i < count; ++i) {
        // Skip drawing if either endpoint is NaN (used for padding gaps)
        if (std::isnan(yValues[i - 1]) || std::isnan(yValues[i])) continue;
        float x0 = dMinX + (float)(i - 1) * stepX;
        float x1 = dMinX + (float)i * stepX;
        ImVec2 p0 = dataToPixel(x0, yValues[i - 1]);
        ImVec2 p1 = dataToPixel(x1, yValues[i]);
        dl->AddLine(p0, p1, color, thickness);
    }
}

void ScopeCanvas::end() {
    ImGui::Dummy(outerSize);
}

ImVec2 ScopeCanvas::dataToPixel(float dataX, float dataY) const {
    float px = graphPos.x + (dataX - dMinX) * scaleX;
    float py = graphPos.y + (graphSize.y - (dataY - dMinY) * scaleY);
    return ImVec2(px, py);
}
