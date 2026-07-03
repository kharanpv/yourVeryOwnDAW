#include "SynthDashboard.h"

#include "elements/ScopeCanvas.h"
#include "elements/ParamBox.h"
#include "elements/ToggleBox.h"
#include "elements/WaveformSelector.h"
#include "elements/WaveformGenerator.h"
#include "elements/EnvelopeGenerator.h"
#include "elements/TerminalStyle.h"

#include <cmath>

// Number of samples in the decimated (display) buffer for 50ms at 44.1kHz
static const int WAVEFORM_DISPLAY_SAMPLES = 2205; // 44100 / 20

SynthDashboard::SynthDashboard(std::shared_ptr<SharedMatrix> matrix,
                                 std::shared_ptr<KeymapRouter> router)
    : dspMatrix(matrix), keyRouter(router),
      paramBox(std::make_unique<ParamBox>()),
      toggleBox(std::make_unique<ToggleBox>()),
      waveformSelector(std::make_unique<WaveformSelector>()),
      scopeCanvas(std::make_unique<ScopeCanvas>()),
      waveGen(std::make_unique<WaveformGenerator>()),
      envGen(std::make_unique<EnvelopeGenerator>()) {
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

    drawWaveformPlane(currentWave);
    drawEnvelopeGraph();
}

// ── WAVEFORM PLANE ─────────────────────────────────────────────────────────
void SynthDashboard::drawWaveformPlane(int waveType) {
    ImGui::BeginChild("WaveformPlane",
                      ImVec2(-1, TerminalStyle::waveformPlaneH()),
                      true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::TextColored(TerminalStyle::textDim(), "WAVEFORM (post-filter)");

    // 1. Generate DSP data
    float fullRate[44100]; // 50ms at 44.1kHz
    float freq = dspMatrix->previewFreq.load();
    float cutoff = dspMatrix->tracks[0].params[P_FILTER_CUTOFF].load();
    float res    = dspMatrix->tracks[0].params[P_FILTER_RES].load();
    float a = dspMatrix->tracks[0].params[P_AMP_ATTACK].load();
    float h = dspMatrix->tracks[0].params[P_AMP_HOLD].load();
    float d = dspMatrix->tracks[0].params[P_AMP_DECAY].load();
    float s = dspMatrix->tracks[0].params[P_AMP_SUSTAIN].load();
    float r = dspMatrix->tracks[0].params[P_AMP_RELEASE].load();

    waveGen->generate(fullRate, 44100, waveType, freq, 44100.0f,
                      cutoff, res, a, h, d, s, r);

    // 2. Decimate to display samples (pick every 20th)
    float display[WAVEFORM_DISPLAY_SAMPLES];
    for (int i = 0; i < WAVEFORM_DISPLAY_SAMPLES; ++i) {
        display[i] = fullRate[i * 20];
    }

    // 3. Draw via ScopeCanvas (time in seconds, amplitude ±1)
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    float cw = ImGui::GetContentRegionAvail().x;
    float ch = ImGui::GetContentRegionAvail().y;

    scopeCanvas->begin(dl, canvasPos, ImVec2(cw, ch),
                       0.0f, 0.05f,   // X: 0ms → 50ms
                       -1.0f, 1.0f);  // Y: -1 → +1
    scopeCanvas->drawCrossHairs();
    scopeCanvas->yLabel("1", 1.0f);
    scopeCanvas->yLabel("0", 0.0f);
    scopeCanvas->yLabel("-1", -1.0f);
    scopeCanvas->xLabel("0ms", 0.0f);
    scopeCanvas->xLabel("25ms", 0.025f);
    scopeCanvas->xLabel("50ms", 0.05f);
    scopeCanvas->plotLine(display, WAVEFORM_DISPLAY_SAMPLES,
                          TerminalStyle::colWhite(),
                          TerminalStyle::lineThickness());
    scopeCanvas->end();

    ImGui::EndChild();
}

// ── ENVELOPE GRAPH ─────────────────────────────────────────────────────────
void SynthDashboard::drawEnvelopeGraph() {
    ImGui::BeginChild("EnvelopeGraph",
                      ImVec2(-1, TerminalStyle::envelopeGraphH()),
                      true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::TextColored(TerminalStyle::textDim(), "AMP ENVELOPE SHAPE");

    float a = dspMatrix->tracks[0].params[P_AMP_ATTACK].load();
    float h = dspMatrix->tracks[0].params[P_AMP_HOLD].load();
    float d = dspMatrix->tracks[0].params[P_AMP_DECAY].load();
    float s = dspMatrix->tracks[0].params[P_AMP_SUSTAIN].load();
    float r = dspMatrix->tracks[0].params[P_AMP_RELEASE].load();

    float envPlot[100];
    envGen->generate(envPlot, 100, a, h, d, s, r);

    // Draw via ScopeCanvas (time in seconds, amplitude 0→1)
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    float cw = ImGui::GetContentRegionAvail().x;
    float ch = ImGui::GetContentRegionAvail().y;

    scopeCanvas->begin(dl, canvasPos, ImVec2(cw, ch),
                       0.0f, 1.0f,  // X: normalized 0→1
                       0.0f, 1.1f); // Y: 0→1.1 (slightly above max)
    scopeCanvas->plotLine(envPlot, 100,
                          TerminalStyle::colAmber(),
                          TerminalStyle::lineThickness());
    scopeCanvas->end();

    ImGui::EndChild();
}