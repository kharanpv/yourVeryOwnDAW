#include "ToggleBox.h"
#include "TerminalStyle.h"

void ToggleBox::draw(const char* label, bool isActive, const std::string& stateStr,
                     const std::string& keyHint) const {
    if (isActive) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, TerminalStyle::activeChildBg());
    }

    ImGui::BeginChild(label, ImVec2(-1, TerminalStyle::paramBoxHeight()), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // Top row: label left, key hint right-anchored
    float hintWidth = ImGui::CalcTextSize(keyHint.c_str()).x;
    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetWindowWidth() - hintWidth - ImGui::GetStyle().WindowPadding.x);
    ImGui::TextColored(TerminalStyle::textHint(), "%s", keyHint.c_str());

    // Bright if active, dim if inactive
    ImVec4 textColor = isActive ? TerminalStyle::amber() : TerminalStyle::textDim();
    ImGui::TextColored(textColor, "> %s", stateStr.c_str());

    ImGui::EndChild();

    if (isActive) {
        ImGui::PopStyleColor();
    }
}