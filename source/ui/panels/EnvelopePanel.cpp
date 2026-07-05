#include "EnvelopePanel.h"
#include "../elements/TerminalStyle.h"

void EnvelopePanel::render(SharedMatrix& matrix) {
    ImGui::BeginChild("EnvelopeGraph",
                      ImVec2(-1, height()),
                      false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::TextColored(TerminalStyle::textDim(), "AMP ENVELOPE SHAPE");

    // 1. Pull AHDSR state from the shared matrix
    float a = matrix.tracks[0].params[P_AMP_ATTACK].load();
    float h = matrix.tracks[0].params[P_AMP_HOLD].load();
    float d = matrix.tracks[0].params[P_AMP_DECAY].load();
    float s = matrix.tracks[0].params[P_AMP_SUSTAIN].load();
    float r = matrix.tracks[0].params[P_AMP_RELEASE].load();

    // 2. Generate envelope shape
    float envPlot[100];
    envGen.generate(envPlot, 100, a, h, d, s, r);

    // 3. Draw via ScopeCanvas (time normalized 0→1, amplitude 0→1)
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    float cw = ImGui::GetContentRegionAvail().x;
    float ch = ImGui::GetContentRegionAvail().y;

    canvas.begin(dl, canvasPos, ImVec2(cw, ch),
                 0.0f, 1.0f,   // X: normalized 0→1
                 0.0f, 1.1f);  // Y: 0→1.1 (slightly above max)
    canvas.showYLabels(false);  // shape is self-explanatory
    canvas.showXLabels(false);
    canvas.drawHorizontalAxis(0.0f);  // baseline at Y=0
    canvas.plotLine(envPlot, 100,
                    TerminalStyle::colAmber(),
                    TerminalStyle::lineThickness());
    canvas.end();

    ImGui::EndChild();
}
