#include "ScopeCanvas.h"
#include "TerminalStyle.h"

void ScopeCanvas::begin(ImDrawList* drawList, ImVec2 pixelPos, ImVec2 pixelSize,
                        float dataMinX, float dataMaxX,
                        float dataMinY, float dataMaxY) {
    dl = drawList;
    pos = pixelPos;
    size = pixelSize;
    dMinX = dataMinX;
    dMaxX = dataMaxX;
    dMinY = dataMinY;
    dMaxY = dataMaxY;

    float dx = dMaxX - dMinX;
    float dy = dMaxY - dMinY;
    scaleX = (dx != 0.0f) ? size.x / dx : 1.0f;
    scaleY = (dy != 0.0f) ? size.y / dy : 1.0f;

    // Black background
    dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                      TerminalStyle::colBlack());
}

void ScopeCanvas::drawAxes() {
    // Horizontal axis at Y = center of data range
    float cy = pos.y + (dMaxY / (dMaxY - dMinY)) * size.y;
    dl->AddLine(ImVec2(pos.x, cy), ImVec2(pos.x + size.x, cy),
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());

    // Vertical axis at X = center of data range
    float cx = pos.x + size.x * 0.5f;
    dl->AddLine(ImVec2(cx, pos.y), ImVec2(cx, pos.y + size.y),
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());
}

void ScopeCanvas::drawCrossHairs() {
    drawAxes();
}

void ScopeCanvas::yLabel(const char* text, float dataY) {
    ImVec2 p = dataToPixel(dMinX, dataY);
    float textW = ImGui::CalcTextSize(text).x;
    dl->AddText(ImVec2(pos.x + 4.0f, p.y - ImGui::GetTextLineHeight() * 0.5f),
                TerminalStyle::colDimText(), text);
}

void ScopeCanvas::xLabel(const char* text, float dataX) {
    ImVec2 p = dataToPixel(dataX, dMinY);
    float textW = ImGui::CalcTextSize(text).x;
    float x = p.x - textW * 0.5f;
    float y = pos.y + size.y - ImGui::GetTextLineHeight() - 2.0f;
    if (x < pos.x + 2.0f) x = pos.x + 2.0f;
    dl->AddText(ImVec2(x, y), TerminalStyle::colDimText(), text);
}

void ScopeCanvas::plotLine(const float* yValues, int count, ImU32 color, float thickness) {
    if (count < 2) return;
    float stepX = (dMaxX - dMinX) / (float)(count - 1);
    for (int i = 1; i < count; ++i) {
        float x0 = dMinX + (float)(i - 1) * stepX;
        float x1 = dMinX + (float)i * stepX;
        ImVec2 p0 = dataToPixel(x0, yValues[i - 1]);
        ImVec2 p1 = dataToPixel(x1, yValues[i]);
        dl->AddLine(p0, p1, color, thickness);
    }
}

void ScopeCanvas::end() {
    // Reserve space in the ImGui layout so the parent window respects our size
    ImGui::Dummy(size);
}

ImVec2 ScopeCanvas::dataToPixel(float dataX, float dataY) const {
    float px = pos.x + (dataX - dMinX) * scaleX;
    float py = pos.y + (dMaxY - dataY) * scaleY; // flip Y: data-up → pixel-down
    return ImVec2(px, py);
}