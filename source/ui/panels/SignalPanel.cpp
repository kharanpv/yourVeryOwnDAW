#include "SignalPanel.h"
#include "../elements/TerminalStyle.h"

void SignalPanel::render(SharedMatrix& matrix) {
    ImGui::BeginChild(label(),
                      ImVec2(-1, height()),
                      false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::TextColored(TerminalStyle::textDim(), label());
    ImGui::Spacing();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 rawPos = ImGui::GetCursorScreenPos();
    float cw = ImGui::GetContentRegionAvail().x;
    float ch = ImGui::GetContentRegionAvail().y;

    // Graph region: inset by LABEL_MARGIN on left and right, shrink bottom for x-labels
    float bottomPad = belowGraphPadding();
    ImVec2 canvasPos(rawPos.x + LABEL_MARGIN, rawPos.y);
    float canvasW = cw - 2.0f * LABEL_MARGIN;
    float canvasH = ch - bottomPad;
    ImVec2 canvasSize(canvasW, canvasH);

    drawBackground(dl, canvasPos, canvasSize);
    drawBorder(dl, canvasPos, canvasSize);

    // Subclass-specific rendering
    drawContent(dl, canvasPos, canvasSize, matrix);

    // Reserve the full child area so ImGui doesn't overlap
    ImGui::Dummy(ImVec2(cw, canvasH + bottomPad));

    ImGui::EndChild();
}

void SignalPanel::drawBackground(ImDrawList* dl, ImVec2 pos, ImVec2 size) {
    dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                      TerminalStyle::colBlack());
}

void SignalPanel::drawBorder(ImDrawList* dl, ImVec2 pos, ImVec2 size) {
    dl->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                TerminalStyle::colBorder(), 0.0f, 0, TerminalStyle::axisThickness());
}
