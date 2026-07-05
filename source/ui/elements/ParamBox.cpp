#include "ParamBox.h"
#include "TerminalStyle.h"

void ParamBox::draw(const char* label, float value, const char* unit,
                    const std::string& keyHint) const {
    ImGui::BeginChild(label, ImVec2(-1, TerminalStyle::paramBoxHeight()), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // Top row: label left, key hint right-anchored
    float hintWidth = ImGui::CalcTextSize(keyHint.c_str()).x;
    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetWindowWidth() - hintWidth - ImGui::GetStyle().WindowPadding.x);
    ImGui::TextColored(TerminalStyle::textHint(), "%s", keyHint.c_str());

    // Bottom row: dim cursor glyph + value
    ImGui::TextColored(TerminalStyle::textDim(), "> %.2f %s", value, unit);

    ImGui::EndChild();
}
