#include "FilterSpectrumPanel.h"
#include "../elements/TerminalStyle.h"

#include <cmath>

void FilterSpectrumPanel::drawContent(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize,
                                      SharedMatrix& matrix) {
    // 1. Pull filter parameters from the shared matrix
    float cutoff    = matrix.tracks[0].params[P_FILTER_CUTOFF].load();
    float resonance = matrix.tracks[0].params[P_FILTER_RES].load();

    // 2. Compute the 2205-point magnitude response
    FilterSpectrumGenerator::Params params;
    params.cutoff    = cutoff;
    params.resonance = resonance;

    float spectrum[FilterSpectrumGenerator::NUM_POINTS];
    FilterSpectrumGenerator::compute(params, spectrum);

    // 3. Coordinate mapping constants (Bode plot — both axes logarithmic)
    static constexpr float F_MIN = 20.0f;
    static constexpr float F_MAX = 20000.0f;
    static const float LOG_RANGE = std::log(F_MAX / F_MIN);
    static constexpr float DB_MIN = -60.0f;
    static constexpr float DB_MAX =  20.0f;
    static constexpr float DB_RANGE = DB_MAX - DB_MIN;

    // 4. Coordinate mapping lambda: data → pixel (log-frequency × dB-amplitude)
    auto toPixel = [&](float freqHz, float dB) -> ImVec2 {
        float normX = std::log(freqHz / F_MIN) / LOG_RANGE;
        float normY = (dB - DB_MIN) / DB_RANGE;
        float px = canvasPos.x + normX * canvasSize.x;
        float py = canvasPos.y + canvasSize.y - normY * canvasSize.y;
        return ImVec2(px, py);
    };

    // Helper: amplitude (linear) → clamped dB
    auto toDB = [](float amp) -> float {
        if (amp <= 0.0f) return DB_MIN;
        float dB = 20.0f * std::log10(amp);
        if (dB < DB_MIN) dB = DB_MIN;
        if (dB > DB_MAX) dB = DB_MAX;
        return dB;
    };

    // 5. Build curve points (dB-mapped) for line and fill polygon
    float stepX = 1.0f / (float)(FilterSpectrumGenerator::NUM_POINTS - 1);
    static constexpr int NUM_VERTS = FilterSpectrumGenerator::NUM_POINTS;
    ImVec2 curveVerts[NUM_VERTS];
    for (int i = 0; i < NUM_VERTS; ++i) {
        float f = F_MIN * std::pow(F_MAX / F_MIN, (float)i * stepX);
        float dB = toDB(spectrum[i]);
        curveVerts[i] = toPixel(f, dB);
    }

    // 6. Translucent fill beneath the curve (same technique as EnvelopePanel)
    //    Polygon: bottom-left → all curve points → bottom-right → close
    static constexpr int FILL_VERTS = NUM_VERTS + 2;
    ImVec2 fillVerts[FILL_VERTS];
    fillVerts[0] = ImVec2(canvasPos.x, canvasPos.y + canvasSize.y);       // bottom-left
    for (int i = 0; i < NUM_VERTS; ++i)
        fillVerts[i + 1] = curveVerts[i];
    fillVerts[FILL_VERTS - 1] = ImVec2(canvasPos.x + canvasSize.x,        // bottom-right
                                        canvasPos.y + canvasSize.y);
    dl->AddConvexPolyFilled(fillVerts, FILL_VERTS,
                            IM_COL32(65, 105, 225, 30));  // ~12% opacity royal blue

    // 7. Y-axis grid lines and labels (dB, evenly spaced)
    auto drawYGrid = [&](const char* text, float dB) {
        ImVec2 left  = toPixel(F_MIN, dB);
        ImVec2 right = toPixel(F_MAX, dB);
        dl->AddLine(left, right,
                    TerminalStyle::colAxis(), TerminalStyle::axisThickness());

        // Tick mark + label in the left margin
        float tick = TerminalStyle::tickLength();
        float half = TerminalStyle::axisThickness() * 0.5f;
        dl->AddLine(ImVec2(canvasPos.x - half, left.y),
                    ImVec2(canvasPos.x - half - tick, left.y),
                    TerminalStyle::colAxis(), TerminalStyle::axisThickness());
        float textW = ImGui::CalcTextSize(text).x;
        dl->AddText(ImVec2(canvasPos.x - tick - 4.0f - textW,
                           left.y - ImGui::GetTextLineHeight() * 0.5f),
                    TerminalStyle::colDimText(), text);
    };
    drawYGrid("+20dB",  20.0f);
    drawYGrid("0dB",     0.0f);
    drawYGrid("-20dB", -20.0f);
    drawYGrid("-40dB", -40.0f);
    drawYGrid("-60dB", -60.0f);

    // 8. X-axis labels (log frequency) below the graph
    float xTick = TerminalStyle::tickLength();
    float xHalf = TerminalStyle::axisThickness() * 0.5f;
    float graphBottom = canvasPos.y + canvasSize.y;

    auto drawXLabel = [&](const char* text, float freqHz) {
        ImVec2 p = toPixel(freqHz, DB_MIN);
        dl->AddLine(ImVec2(p.x, graphBottom + xHalf),
                    ImVec2(p.x, graphBottom + xHalf + xTick),
                    TerminalStyle::colAxis(), TerminalStyle::axisThickness());
        float textW = ImGui::CalcTextSize(text).x;
        dl->AddText(ImVec2(p.x - textW * 0.5f, graphBottom + xHalf + xTick + 2.0f),
                    TerminalStyle::colDimText(), text);
    };
    drawXLabel("20Hz",  20.0f);
    drawXLabel("200Hz", 200.0f);
    drawXLabel("2kHz",  2000.0f);
    drawXLabel("20kHz", 20000.0f);

    // 9. Plot the filter transfer function line (royal blue, on top of fill)
    for (int i = 1; i < NUM_VERTS; ++i) {
        dl->AddLine(curveVerts[i - 1], curveVerts[i],
                    COL_FILTER_LINE, TerminalStyle::lineThickness());
    }
}
