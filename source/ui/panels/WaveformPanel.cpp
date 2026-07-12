#include "WaveformPanel.h"
#include "../elements/TerminalStyle.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void WaveformPanel::drawContent(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize,
                                 SharedMatrix& matrix) {
    int waveType = (int)matrix.tracks[0].params[P_OSC_WAVEFORM].load();
    if (waveType < 0 || waveType > 4) waveType = 1;

    // 1. Generate a single-cycle wavetable (raw oscillator, no filter).
    float wavetable[WAVETABLE_SIZE];
    waveGen.generate(wavetable, WAVETABLE_SIZE, waveType,
                     1.0f,      // freqHz - arbitrary, shape only
                     2205.0f,   // sampleRate - one cycle = WAVETABLE_SIZE
                     20000.0f,  // cutoff - wide open, no filtering
                     0.0f,      // resonance - zero
                     0.0f, 0.0f, 0.0f, 1.0f, 0.0f);  // sustain-only

    // 2. Build padded buffer: [100 NaN] [0] [2205 wave] [0] [100 NaN]
    static constexpr int TOTAL = WAVETABLE_SIZE + 2 + 2 * PAD_SAMPLES;
    float padded[TOTAL];
    for (int i = 0; i < PAD_SAMPLES; ++i)
        padded[i] = NAN;
    padded[PAD_SAMPLES] = 0.0f;
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
        padded[PAD_SAMPLES + 1 + i] = wavetable[i];
    padded[PAD_SAMPLES + 1 + WAVETABLE_SIZE] = 0.0f;
    for (int i = 0; i < PAD_SAMPLES; ++i)
        padded[PAD_SAMPLES + 1 + WAVETABLE_SIZE + 1 + i] = NAN;

    // 3. Data coordinate ranges
    float sampleSpacing = 2.0f * (float)M_PI / (float)WAVETABLE_SIZE;
    float xMin = -sampleSpacing - PAD_SAMPLES * sampleSpacing;
    float xMax = 2.0f * (float)M_PI + sampleSpacing + PAD_SAMPLES * sampleSpacing;
    float yMin = -1.0f, yMax = 1.0f;

    // 4. Coordinate mapping lambda
    auto toPixel = [&](float dataX, float dataY) -> ImVec2 {
        float px = canvasPos.x + (dataX - xMin) / (xMax - xMin) * canvasSize.x;
        float py = canvasPos.y + canvasSize.y - (dataY - yMin) / (yMax - yMin) * canvasSize.y;
        return ImVec2(px, py);
    };

    // 5. Horizontal axis at Y=0
    dl->AddLine(toPixel(xMin, 0.0f), toPixel(xMax, 0.0f),
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());

    // 6. Vertical axis at X=pi
    dl->AddLine(toPixel((float)M_PI, yMax), toPixel((float)M_PI, yMin),
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());

    // 7. Y-axis labels
    auto drawYLabel = [&](const char* text, float dataY) {
        ImVec2 p = toPixel(xMin, dataY);
        float tick = TerminalStyle::tickLength();
        dl->AddLine(ImVec2(canvasPos.x - TerminalStyle::axisThickness() * 0.5f, p.y),
                    ImVec2(canvasPos.x - tick, p.y),
                    TerminalStyle::colAxis(), TerminalStyle::axisThickness());
        float textW = ImGui::CalcTextSize(text).x;
        dl->AddText(ImVec2(canvasPos.x - tick - 4.0f - textW,
                           p.y - ImGui::GetTextLineHeight() * 0.5f),
                    TerminalStyle::colDimText(), text);
    };
    drawYLabel("1", 1.0f);
    drawYLabel("0", 0.0f);
    drawYLabel("-1", -1.0f);

    // 8. X-axis labels (below graph)
    float tick = TerminalStyle::tickLength();
    float half = TerminalStyle::axisThickness() * 0.5f;
    float graphBottom = canvasPos.y + canvasSize.y;

    auto drawXLabel = [&](const char* text, float dataX) {
        ImVec2 p = toPixel(dataX, yMin);
        dl->AddLine(ImVec2(p.x, graphBottom + half),
                    ImVec2(p.x, graphBottom + half + tick),
                    TerminalStyle::colAxis(), TerminalStyle::axisThickness());
        float textW = ImGui::CalcTextSize(text).x;
        dl->AddText(ImVec2(p.x - textW * 0.5f, graphBottom + half + tick + 2.0f),
                    TerminalStyle::colDimText(), text);
    };
    drawXLabel("0", 0.0f);
    drawXLabel("pi", (float)M_PI);
    drawXLabel("2pi", 2.0f * (float)M_PI);

    // 9. Plot the waveform line
    float stepX = (xMax - xMin) / (float)(TOTAL - 1);
    for (int i = 1; i < TOTAL; ++i) {
        if (std::isnan(padded[i - 1]) || std::isnan(padded[i])) continue;
        ImVec2 p0 = toPixel(xMin + (float)(i - 1) * stepX, padded[i - 1]);
        ImVec2 p1 = toPixel(xMin + (float)i * stepX, padded[i]);
        dl->AddLine(p0, p1, TerminalStyle::colWhite(), TerminalStyle::lineThickness());
    }
}
