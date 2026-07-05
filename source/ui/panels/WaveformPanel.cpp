#include "WaveformPanel.h"
#include "../elements/TerminalStyle.h"

#include <cmath>

void WaveformPanel::render(SharedMatrix& matrix, int waveType) {
    ImGui::BeginChild("WaveformPlane",
                      ImVec2(-1, height()),
                      false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::TextColored(TerminalStyle::textDim(), "WAVEFORM (post-filter)");

    // 1. Pull DSP state from the shared matrix
    float freq   = matrix.previewFreq.load();
    float cutoff = matrix.tracks[0].params[P_FILTER_CUTOFF].load();
    float res    = matrix.tracks[0].params[P_FILTER_RES].load();
    float a = matrix.tracks[0].params[P_AMP_ATTACK].load();
    float h = matrix.tracks[0].params[P_AMP_HOLD].load();
    float d = matrix.tracks[0].params[P_AMP_DECAY].load();
    float s = matrix.tracks[0].params[P_AMP_SUSTAIN].load();
    float r = matrix.tracks[0].params[P_AMP_RELEASE].load();

    // 2. Generate full-rate waveform (50ms at 44.1kHz)
    float fullRate[44100];
    waveGen.generate(fullRate, 44100, waveType, freq, 44100.0f,
                     cutoff, res, a, h, d, s, r);

    // 3. Decimate to display samples (pick every 20th)
    float display[WAVEFORM_DISPLAY_SAMPLES];
    for (int i = 0; i < WAVEFORM_DISPLAY_SAMPLES; ++i) {
        display[i] = fullRate[i * 20];
    }

    // 4. Draw via ScopeCanvas (time in seconds, amplitude ±1)
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    float cw = ImGui::GetContentRegionAvail().x;
    float ch = ImGui::GetContentRegionAvail().y;

    canvas.begin(dl, canvasPos, ImVec2(cw, ch),
                 0.0f, 0.05f,   // X: 0ms → 50ms
                 -1.0f, 1.0f);  // Y: -1 → +1
    canvas.showYLabels(true);
    canvas.showXLabels(false);   // time labels not useful here
    canvas.drawHorizontalAxis(0.0f);
    canvas.drawVerticalAxis(0.025f);
    canvas.yLabel("1", 1.0f);
    canvas.yLabel("0", 0.0f);
    canvas.yLabel("-1", -1.0f);
    canvas.plotLine(display, WAVEFORM_DISPLAY_SAMPLES,
                    TerminalStyle::colWhite(),
                    TerminalStyle::lineThickness());
    canvas.end();

    ImGui::EndChild();
}
