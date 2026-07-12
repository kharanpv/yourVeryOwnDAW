#include "EnvelopePanel.h"
#include "../elements/TerminalStyle.h"

void EnvelopePanel::drawContent(ImDrawList* dl, ImVec2 canvasPos, ImVec2 canvasSize,
                                 SharedMatrix& matrix) {
    // 1. Pull AHDSR state from the shared matrix
    float a = matrix.tracks[0].params[P_AMP_ATTACK].load();
    float h = matrix.tracks[0].params[P_AMP_HOLD].load();
    float d = matrix.tracks[0].params[P_AMP_DECAY].load();
    float s = matrix.tracks[0].params[P_AMP_SUSTAIN].load();
    float r = matrix.tracks[0].params[P_AMP_RELEASE].load();

    // 2. Compute the 6 envelope nodes in screen-space coordinates
    EnvelopeNodes nodes = envGen.compute(canvasPos, canvasSize, a, h, d, s, r);

    // 3. Horizontal bisecting line at 0.5 amplitude
    float midY = canvasPos.y + canvasSize.y * 0.5f;
    dl->AddLine(ImVec2(canvasPos.x, midY),
                ImVec2(canvasPos.x + canvasSize.x, midY),
                TerminalStyle::colAxis(), TerminalStyle::axisThickness());

    // 4. Y-axis labels in the left margin
    float tick = TerminalStyle::tickLength();
    float halfThick = TerminalStyle::axisThickness() * 0.5f;

    auto drawYLabel = [&](const char* text, float amplitude) {
        float y = canvasPos.y + canvasSize.y - amplitude * canvasSize.y;
        dl->AddLine(ImVec2(canvasPos.x - halfThick, y),
                    ImVec2(canvasPos.x - tick, y),
                    TerminalStyle::colAxis(), TerminalStyle::axisThickness());
        float textW = ImGui::CalcTextSize(text).x;
        dl->AddText(ImVec2(canvasPos.x - tick - 4.0f - textW,
                           y - ImGui::GetTextLineHeight() * 0.5f),
                    TerminalStyle::colDimText(), text);
    };
    drawYLabel("1", 1.0f);
    drawYLabel("0.5", 0.5f);
    drawYLabel("0", 0.0f);

    // 5. Filled region under the envelope (ambient glow)
    ImVec2 fill[8];
    fill[0] = nodes.P0;
    fill[1] = nodes.P1;
    fill[2] = nodes.P2;
    fill[3] = nodes.P3;
    fill[4] = nodes.P4;
    fill[5] = nodes.P5;
    fill[6] = ImVec2(nodes.P5.x, canvasPos.y + canvasSize.y);
    fill[7] = ImVec2(nodes.P0.x, canvasPos.y + canvasSize.y);
    dl->AddConvexPolyFilled(fill, 8, IM_COL32(255, 178, 0, 25));

    // 6. The envelope line itself
    ImVec2 linePoints[6] = { nodes.P0, nodes.P1, nodes.P2,
                             nodes.P3, nodes.P4, nodes.P5 };
    dl->AddPolyline(linePoints, 6,
                    TerminalStyle::colAmber(), 0,
                    TerminalStyle::lineThickness());
}
