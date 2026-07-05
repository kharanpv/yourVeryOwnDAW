#include "SynthDashboard.h"

#include "elements/ParamBox.h"
#include "elements/ToggleBox.h"
#include "elements/WaveformSelector.h"
#include "elements/TerminalStyle.h"
#include "panels/WaveformPanel.h"
#include "panels/EnvelopePanel.h"

#include <cmath>

SynthDashboard::SynthDashboard(std::shared_ptr<SharedMatrix> matrix,
                                 std::shared_ptr<KeymapRouter> router)
    : dspMatrix(matrix), keyRouter(router),
      paramBox(std::make_unique<ParamBox>()),
      toggleBox(std::make_unique<ToggleBox>()),
      waveformSelector(std::make_unique<WaveformSelector>()),
      waveformPanel(std::make_unique<WaveformPanel>()),
      envelopePanel(std::make_unique<EnvelopePanel>()) {
    setupTerminalTheme();
}

SynthDashboard::~SynthDashboard() = default;

void SynthDashboard::setupTerminalTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = TerminalStyle::childBg();
    style.Colors[ImGuiCol_Border]   = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_Text]     = TerminalStyle::textWhite();
    style.WindowPadding = ImVec2(TerminalStyle::windowPadding(),
                                  TerminalStyle::windowPadding());
}

void SynthDashboard::render() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Terminal_HUD", nullptr, flags);

    if (ImGui::BeginTable("HudLayout", 2, ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Parameters", ImGuiTableColumnFlags_WidthStretch, 0.33f);
        ImGui::TableSetupColumn("Telemetry",  ImGuiTableColumnFlags_WidthStretch, 0.603f);
        ImGui::TableNextRow();

        int currentWave = (int)dspMatrix->tracks[0].params[P_OSC_WAVEFORM].load();
        if (currentWave < 0 || currentWave > 4) currentWave = 1;

        ImGui::TableSetColumnIndex(0);
        drawLeftColumn(currentWave);

        ImGui::TableSetColumnIndex(1);
        drawRightColumn(currentWave);

        ImGui::EndTable();
    }

    ImGui::End();
}

// ── LEFT COLUMN ────────────────────────────────────────────────────────────
void SynthDashboard::drawLeftColumn(int currentWave) {
    ImGui::TextColored(TerminalStyle::accent(), "SYSTEM // PARAMETERS");
    ImGui::Separator();
    ImGui::Spacing();

    // Waveform selector
    const char* keyHints[5] = { "1", "2", "3", "4", "5" };
    waveformSelector->draw(currentWave, keyHints);

    // OSC FREQ (read-only)
    float previewFreq = dspMatrix->previewFreq.load();
    std::string oscKey = keyRouter->getKeyName(GrooveboxAction::CUTOFF_UP);
    std::string oscHint = "[" + oscKey + "]";
    paramBox->draw("OSC FREQ", previewFreq, "Hz", oscHint);

    // Parameter boxes
    auto cutoff = dspMatrix->tracks[0].params[P_FILTER_CUTOFF].load();
    std::string cutUp   = keyRouter->getKeyName(GrooveboxAction::CUTOFF_UP);
    std::string cutDown = keyRouter->getKeyName(GrooveboxAction::CUTOFF_DOWN);
    std::string cutHint = "[" + cutUp + "]/[" + cutDown + "]";
    paramBox->draw("FILTER CUTOFF", cutoff, "Hz", cutHint);

    auto res = dspMatrix->tracks[0].params[P_FILTER_RES].load();
    std::string resUp   = keyRouter->getKeyName(GrooveboxAction::RES_UP);
    std::string resDown = keyRouter->getKeyName(GrooveboxAction::RES_DOWN);
    std::string resHint = "[" + resUp + "]/[" + resDown + "]";
    paramBox->draw("FILTER RES", res, "%", resHint);

    auto attack = dspMatrix->tracks[0].params[P_AMP_ATTACK].load();
    std::string attUp   = keyRouter->getKeyName(GrooveboxAction::ATTACK_UP);
    std::string attDown = keyRouter->getKeyName(GrooveboxAction::ATTACK_DOWN);
    std::string attHint = "[" + attUp + "]/[" + attDown + "]";
    paramBox->draw("AMP ATTACK", attack, "sec", attHint);

    auto hold = dspMatrix->tracks[0].params[P_AMP_HOLD].load();
    std::string holdUp   = keyRouter->getKeyName(GrooveboxAction::HOLD_UP);
    std::string holdDown = keyRouter->getKeyName(GrooveboxAction::HOLD_DOWN);
    std::string holdHint = "[" + holdUp + "]/[" + holdDown + "]";
    paramBox->draw("AMP HOLD", hold, "sec", holdHint);

    auto decay = dspMatrix->tracks[0].params[P_AMP_DECAY].load();
    std::string decUp   = keyRouter->getKeyName(GrooveboxAction::DECAY_UP);
    std::string decDown = keyRouter->getKeyName(GrooveboxAction::DECAY_DOWN);
    std::string decHint = "[" + decUp + "]/[" + decDown + "]";
    paramBox->draw("AMP DECAY", decay, "sec", decHint);

    auto sustain = dspMatrix->tracks[0].params[P_AMP_SUSTAIN].load();
    std::string susUp   = keyRouter->getKeyName(GrooveboxAction::SUSTAIN_UP);
    std::string susDown = keyRouter->getKeyName(GrooveboxAction::SUSTAIN_DOWN);
    std::string susHint = "[" + susUp + "]/[" + susDown + "]";
    paramBox->draw("AMP SUSTAIN", sustain * 100.0f, "%", susHint);

    auto release = dspMatrix->tracks[0].params[P_AMP_RELEASE].load();
    std::string relUp   = keyRouter->getKeyName(GrooveboxAction::RELEASE_UP);
    std::string relDown = keyRouter->getKeyName(GrooveboxAction::RELEASE_DOWN);
    std::string relHint = "[" + relUp + "]/[" + relDown + "]";
    paramBox->draw("AMP RELEASE", release, "sec", relHint);

    // Latch toggle
    bool isLatched = dspMatrix->tracks[0].params[P_LATCH_MODE].load() > 0.5f;
    std::string latchKey = keyRouter->getKeyName(GrooveboxAction::TOGGLE_LATCH);
    std::string latchHint = "[" + latchKey + "]";
    toggleBox->draw("LATCH MODE", isLatched,
                    isLatched ? "ENGAGED" : "OFF", latchHint);
}

// ── RIGHT COLUMN ───────────────────────────────────────────────────────────
void SynthDashboard::drawRightColumn(int currentWave) {
    ImGui::TextColored(TerminalStyle::accent(), "TELEMETRY // SIGNAL");
    ImGui::Separator();
    ImGui::Spacing();

    // Each panel is a fully self-contained rendering unit.
    // Adding a third panel here is one line — no other code changes needed.
    waveformPanel->render(*dspMatrix, currentWave);
    envelopePanel->render(*dspMatrix);
}