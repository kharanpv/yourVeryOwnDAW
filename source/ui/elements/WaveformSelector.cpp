#include "WaveformSelector.h"
#include "TerminalStyle.h"
#include <string>

constexpr const char* WaveformSelector::NAMES[NUM_WAVEFORMS];

void WaveformSelector::draw(int currentWave, const char* keyHints[NUM_WAVEFORMS]) const {
    float height = TerminalStyle::windowPadding() + NUM_WAVEFORMS * TerminalStyle::selectorItemH();
    ImGui::BeginChild("WaveSelect", ImVec2(-1, height), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::Text("%s", "WAVEFORM");

    for (int i = 0; i < NUM_WAVEFORMS; ++i) {
        bool isActive = (i == currentWave);
        std::string line = std::string("> ") + NAMES[i];

        if (isActive)
            ImGui::TextColored(TerminalStyle::magenta(), "%s", line.c_str());
        else
            ImGui::TextColored(TerminalStyle::textDim(), "%s", line.c_str());

        // Right-anchor the key hint
        std::string kh(keyHints[i] ? keyHints[i] : "");
        if (!kh.empty()) {
            std::string hint = "[" + kh + "]";
            float hintWidth = ImGui::CalcTextSize(hint.c_str()).x;
            ImGui::SameLine(ImGui::GetWindowWidth() - hintWidth - ImGui::GetStyle().WindowPadding.x);
            ImGui::TextColored(TerminalStyle::textHint(), "%s", hint.c_str());
        }
    }

    ImGui::EndChild();
}

void WaveformSelector::draw(int currentWave, const std::string keyHints[NUM_WAVEFORMS]) const {
    const char* cstrs[NUM_WAVEFORMS];
    for (int i = 0; i < NUM_WAVEFORMS; ++i) {
        cstrs[i] = keyHints[i].c_str();
    }
    draw(currentWave, cstrs);
}
