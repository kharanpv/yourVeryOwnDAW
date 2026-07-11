#include "WaveformPanel.h"
#include "../elements/TerminalStyle.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void WaveformPanel::render(SharedMatrix& matrix, int waveType) {
    ImGui::BeginChild("WaveformPlane",
                      ImVec2(-1, height()),
                      false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::TextColored(TerminalStyle::textDim(), "WAVEFORM");

    // 1. Generate a single-cycle wavetable (raw oscillator, no filter).
    //    Cutoff wide-open and resonance zero so the filter has no effect.
    float wavetable[WAVETABLE_SIZE];
    waveGen.generate(wavetable, WAVETABLE_SIZE, waveType,
                     1.0f,      // freqHz - arbitrary, shape only
                     2205.0f,   // sampleRate - one cycle = WAVETABLE_SIZE
                     20000.0f,  // cutoff - wide open, no filtering
                     0.0f,      // resonance - zero
                     0.0f, 0.0f, 0.0f, 1.0f, 0.0f);  // sustain-only

    // 2. Build padded buffer: [100 NaN] [0] [2205 wave] [0] [100 NaN]
    //    Leading and trailing 0s draw vertical lines at discontinuities
    //    (saw, square) without extending past the cycle boundary.
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

    // 3. Compute X range: extra sample on each side for vertical lines
    float sampleSpacing = 2.0f * (float)M_PI / (float)WAVETABLE_SIZE;
    float xMin = -sampleSpacing - PAD_SAMPLES * sampleSpacing;
    float xMax = 2.0f * (float)M_PI + sampleSpacing + PAD_SAMPLES * sampleSpacing;

    // 4. Draw
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    float cw = ImGui::GetContentRegionAvail().x;
    float ch = ImGui::GetContentRegionAvail().y;

    canvas.begin(dl, canvasPos, ImVec2(cw, ch),
                 xMin, xMax,
                 -1.0f, 1.0f);
    canvas.showYLabels(true);
    canvas.showXLabels(true);
    canvas.drawHorizontalAxis(0.0f);
    canvas.drawVerticalAxis((float)M_PI);
    canvas.yLabel("1", 1.0f);
    canvas.yLabel("0", 0.0f);
    canvas.yLabel("-1", -1.0f);
    canvas.xLabel("0", 0.0f);
    canvas.xLabel("pi", (float)M_PI);
    canvas.xLabel("2pi", 2.0f * (float)M_PI);
    canvas.plotLine(padded, TOTAL,
                    TerminalStyle::colWhite(),
                    TerminalStyle::lineThickness());
    canvas.end();

    ImGui::EndChild();
}
