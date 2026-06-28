#include "SynthDashboard.h"
#include <cmath>
#include <cstdlib> // for rand()

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SynthDashboard::SynthDashboard(std::shared_ptr<SharedMatrix> matrix, std::shared_ptr<KeymapRouter> router)
    : dspMatrix(matrix), keyRouter(router) {
    setupTerminalTheme();
}

void SynthDashboard::setupTerminalTheme() {
    // 80s/90s High-Res Terminal Aesthetic
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;       // Sharp, blocky corners
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f); // Void Black
    style.Colors[ImGuiCol_Border]   = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); // Dark Grey borders
    style.Colors[ImGuiCol_Text]     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // Crisp White

    // Uniform 8px padding on all sides of child windows
    style.WindowPadding = ImVec2(8.0f, 8.0f);
}

void SynthDashboard::render() {
    // 1. Create a monolithic, unmovable HUD covering the entire OS window
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    // Strip away all OS-level window decorations
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Terminal_HUD", nullptr, flags);

    // 2. Define the Layout (1/3 Left Column, 2/3 Right Column)
    if (ImGui::BeginTable("HudLayout", 2, ImGuiTableFlags_BordersInnerV)) {

        // Lock the left column to exactly 33% of the screen width
        ImGui::TableSetupColumn("Parameters", ImGuiTableColumnFlags_WidthStretch, 0.33f);
        ImGui::TableSetupColumn("Telemetry", ImGuiTableColumnFlags_WidthStretch, 0.67f);

        ImGui::TableNextRow();

        // ==========================================
        // LEFT COLUMN: THE PARAMETER BOXES
        // ==========================================
        ImGui::TableSetColumnIndex(0);

        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.8f, 1.0f), "SYSTEM // PARAMETERS"); 
        ImGui::Separator();
        ImGui::Spacing();

        // --- Waveform Selector (read-only display, keyboard-only control) ---
        int currentWave = (int)dspMatrix->tracks[0].params[P_OSC_WAVEFORM].load();
        if (currentWave < 0 || currentWave > 4) currentWave = 1;
        drawWaveformSelector(currentWave);

        ImGui::Spacing();

        drawContinuousBox("FILTER CUTOFF", dspMatrix->tracks[0].params[P_FILTER_CUTOFF].load(), "Hz",
                            GrooveboxAction::CUTOFF_UP, GrooveboxAction::CUTOFF_DOWN);

        drawContinuousBox("FILTER RES", dspMatrix->tracks[0].params[P_FILTER_RES].load(), "%",
                            GrooveboxAction::RES_UP, GrooveboxAction::RES_DOWN);

        drawContinuousBox("AMP ATTACK", dspMatrix->tracks[0].params[P_AMP_ATTACK].load(), "sec",
                            GrooveboxAction::ATTACK_UP, GrooveboxAction::ATTACK_DOWN);

        ImGui::Spacing();

        // Convert the float parameter back into a boolean for the UI display
        bool isLatched = dspMatrix->tracks[0].params[P_LATCH_MODE].load() > 0.5f;
        drawToggleBox("LATCH MODE", isLatched, isLatched ? "ENGAGED" : "OFF", GrooveboxAction::TOGGLE_LATCH);


        // ==========================================
        // RIGHT COLUMN: TELEMETRY & VISUALIZERS
        // ==========================================
        ImGui::TableSetColumnIndex(1);
        
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.8f, 1.0f), "TELEMETRY // SIGNAL");
        ImGui::Separator();
        ImGui::Spacing();

        // --- 1. The Live Oscilloscope ---
        ImGui::BeginChild("Oscilloscope", ImVec2(-1, 200), true);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "LIVE WAVEFORM (post-filter)");
        
        // Safely extract the atomic data into a local array for ImGui to draw
        float localScope[SharedMatrix::SCOPE_SIZE];
        int scopeCount = dspMatrix->scopeWriteIndex.load(std::memory_order_relaxed);
        if (scopeCount > SharedMatrix::SCOPE_SIZE) scopeCount = SharedMatrix::SCOPE_SIZE;
        
        for (int i = 0; i < scopeCount; ++i) {
            localScope[i] = dspMatrix->oscilloscopeBuffer[i].load(std::memory_order_relaxed);
        }
        // Fill trailing samples with 0 if we haven't filled the buffer yet
        for (int i = scopeCount; i < SharedMatrix::SCOPE_SIZE; ++i) {
            localScope[i] = 0.0f;
        }
        
        // Draw the neon wave (Cyan)
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.8f, 1.0f));
        if (scopeCount > 0) {
            ImGui::PlotLines("##Scope", localScope, SharedMatrix::SCOPE_SIZE, 0, nullptr, -1.0f, 1.0f, ImVec2(-1, 150));
        } else {
            // No data yet — show the static preview instead
            float previewBuf[256];
            float cutoff = dspMatrix->tracks[0].params[P_FILTER_CUTOFF].load();
            float res    = dspMatrix->tracks[0].params[P_FILTER_RES].load();
            generateWaveformPreview(previewBuf, 256, currentWave, cutoff, res, 44100.0f);
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::PlotLines("##Preview", previewBuf, 256, 0, nullptr, -1.0f, 1.0f, ImVec2(-1, 150));
            ImGui::PopStyleColor();
        }
        ImGui::PopStyleColor();
        ImGui::EndChild();

        ImGui::Spacing();

            // --- 1b. Static Waveform Preview (shows the unfiltered wave shape)
            //     This is always visible and changes with the selected waveform + filter params
            ImGui::BeginChild("WaveformPreview", ImVec2(-1, 130), true);
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "WAVEFORM PREVIEW");

            float previewBuf[256];
            float cutoff = dspMatrix->tracks[0].params[P_FILTER_CUTOFF].load();
            float res    = dspMatrix->tracks[0].params[P_FILTER_RES].load();
            generateWaveformPreview(previewBuf, 256, currentWave, cutoff, res, 44100.0f);

            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.2f, 0.8f, 1.0f, 1.0f)); // Light blue
            ImGui::PlotLines("##Preview", previewBuf, 256, 0, nullptr, -1.0f, 1.0f, ImVec2(-1, 90));
            ImGui::PopStyleColor();
            ImGui::EndChild();

            ImGui::Spacing();

            // --- 2. The Envelope Graph (Amp ADSR) ---
        ImGui::BeginChild("EnvelopeGraph", ImVec2(-1, 150), true);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "AMP ENVELOPE SHAPE");

        // Dynamically calculate a 100-point graph based on current ADSR values
        float envPlot[100];
        float a = dspMatrix->tracks[0].params[P_AMP_ATTACK].load();
        float d = dspMatrix->tracks[0].params[P_AMP_DECAY].load();
        float s = dspMatrix->tracks[0].params[P_AMP_SUSTAIN].load();
        float r = dspMatrix->tracks[0].params[P_AMP_RELEASE].load();
        
        // Map the physical seconds to graph proportions (simplified for UI logic)
        float totalTime = a + d + r + 0.1f; // 0.1f is a dummy "Sustain/Hold" time for the graphic
        int attackSamples = (int)((a / totalTime) * 100);
        int decaySamples  = (int)((d / totalTime) * 100);
        int holdSamples   = (int)((0.1f / totalTime) * 100);
        int releaseSamples = 100 - (attackSamples + decaySamples + holdSamples);

        int idx = 0;
        // Attack slope
        for (int i = 0; i < attackSamples && idx < 100; ++i) envPlot[idx++] = (float)i / attackSamples;
        // Decay slope
        for (int i = 0; i < decaySamples && idx < 100; ++i)  envPlot[idx++] = 1.0f - ((1.0f - s) * ((float)i / decaySamples));
        // Sustain plateau
        for (int i = 0; i < holdSamples && idx < 100; ++i)   envPlot[idx++] = s;
        // Release slope
        for (int i = 0; i < releaseSamples && idx < 100; ++i) envPlot[idx++] = s * (1.0f - ((float)i / releaseSamples));

        // Fill any rounding remainders
        while(idx < 100) envPlot[idx++] = 0.0f;

        // Draw the neon envelope (Amber)
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.7f, 0.0f, 1.0f));
        ImGui::PlotLines("##ADSR", envPlot, 100, 0, nullptr, 0.0f, 1.1f, ImVec2(-1, 100));
        ImGui::PopStyleColor();
        
        ImGui::EndChild();

        ImGui::EndTable();
    }

    ImGui::End();
}

// --- Custom Widget Drawing Functions ---

void SynthDashboard::drawContinuousBox(const char* label, float value, const char* unit,
                                       GrooveboxAction actionUp, GrooveboxAction actionDown) {

    std::string keyUp   = keyRouter->getKeyName(actionUp);
    std::string keyDown = keyRouter->getKeyName(actionDown);
    std::string keyHint = std::string("[") + keyUp + "]/[" + keyDown + "]";

    ImGui::BeginChild(label, ImVec2(-1, 55), true, ImGuiWindowFlags_NoScrollbar);

    // Top row: label left, key hint right-anchored
    float hintWidth = ImGui::CalcTextSize(keyHint.c_str()).x;
    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetWindowWidth() - hintWidth - ImGui::GetStyle().WindowPadding.x);
    ImGui::TextColored(ImVec4(0.35f, 0.35f, 0.35f, 1.0f), "%s", keyHint.c_str());

    // Bottom row: dim cursor glyph + value — no active highlighting
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "> %.2f %s", value, unit);

    ImGui::EndChild();
}

void SynthDashboard::drawToggleBox(const char* label, bool isActive, const std::string& stateStr,
                                   GrooveboxAction toggleAction) {

    std::string key = keyRouter->getKeyName(toggleAction);
    std::string keyHint = std::string("[") + key + "]";

    // If active, invert the background color of the box for high visual feedback
    if (isActive) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    }

    ImGui::BeginChild(label, ImVec2(-1, 55), true, ImGuiWindowFlags_NoScrollbar);

    // Top row: label left, key hint right-anchored (same method as continuous boxes)
    float hintWidth = ImGui::CalcTextSize(keyHint.c_str()).x;
    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetWindowWidth() - hintWidth - ImGui::GetStyle().WindowPadding.x);
    ImGui::TextColored(ImVec4(0.35f, 0.35f, 0.35f, 1.0f), "%s", keyHint.c_str());

    // Bright color if active, dim if inactive
    ImVec4 textColor = isActive ? ImVec4(1.0f, 0.7f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImGui::TextColored(textColor, "> %s", stateStr.c_str());

    ImGui::EndChild();

    if (isActive) {
        ImGui::PopStyleColor(); // Restore normal background
    }
}

void SynthDashboard::drawWaveformSelector(int currentWave) {
    const char* names[5] = { "SINE", "SAW", "SQUARE", "TRIANGLE", "NOISE" };
    const char* keyNames[5] = { "1", "2", "3", "4", "5" };
    const ImVec4 magenta(1.0f, 0.2f, 0.8f, 1.0f);
    const ImVec4 dim(0.5f, 0.5f, 0.5f, 1.0f);
    const ImVec4 keyDim(0.35f, 0.35f, 0.35f, 1.0f);

    ImGui::BeginChild("WaveSelect", ImVec2(-1, 10 + 5 * 22), true, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("%s", "WAVEFORM");

    for (int i = 0; i < 5; ++i) {
        bool isActive = (i == currentWave);
        std::string line = std::string("> ") + names[i];

        if (isActive)
            ImGui::TextColored(magenta, "%s", line.c_str());
        else
            ImGui::TextColored(dim, "%s", line.c_str());

        // Right-anchor the key hint, same technique as drawContinuousBox
        std::string keyHint = std::string("[") + keyNames[i] + "]";
        float hintWidth = ImGui::CalcTextSize(keyHint.c_str()).x;
        ImGui::SameLine(ImGui::GetWindowWidth() - hintWidth - ImGui::GetStyle().WindowPadding.x);
        ImGui::TextColored(keyDim, "%s", keyHint.c_str());
    }

    ImGui::EndChild();
}

void SynthDashboard::generateWaveformPreview(float* outBuffer, int numSamples,
                                              int waveType, float cutoffHz,
                                              float resonance, float sampleRate) {
    // Generate multiple cycles of the raw waveform, then pass it through the
    // same 4-pole cascaded low-pass filter that the audio engine uses.
    // Showing multiple cycles makes the waveform shape immediately recognizable.

    const float numCycles = 8.0f;

    // --- Step 1: generate the raw waveform ---
    for (int i = 0; i < numSamples; ++i) {
        float phase = std::fmod((float)i / (float)numSamples * numCycles, 1.0f);
        float raw = 0.0f;

        switch (waveType) {
            case 0: // Sine
                raw = std::sin(phase * 2.0f * M_PI);
                break;
            case 1: // Saw
                raw = 2.0f * phase - 1.0f;
                break;
            case 2: // Square
                raw = (phase < 0.5f) ? 1.0f : -1.0f;
                break;
            case 3: // Triangle
                raw = 4.0f * std::abs(phase - 0.5f) - 1.0f;
                break;
            case 4: // Noise (seed by sample index to be deterministic for preview)
                raw = ((float)((i * 12345 + 6789) % 1000) / 500.0f) - 1.0f;
                break;
        }

        outBuffer[i] = raw;
    }

    // --- Step 2: apply the actual 4-pole cascade filter (same as LowPassFilter) ---
    // Clamp cutoff
    if (cutoffHz < 20.0f) cutoffHz = 20.0f;
    if (cutoffHz > 20000.0f) cutoffHz = 20000.0f;
    if (resonance < 0.0f) resonance = 0.0f;
    if (resonance > 3.99f) resonance = 3.99f;

    // Calculate 'g' coefficient the same way LowPassFilter::setCutoff does
    float g = 1.0f - std::exp(-2.0f * (float)M_PI * cutoffHz / sampleRate);

    // Run the 4-pole cascade
    float s1 = 0.0f, s2 = 0.0f, s3 = 0.0f, s4 = 0.0f;
    for (int i = 0; i < numSamples; ++i) {
        float resonatedInput = outBuffer[i] - (s4 * resonance);
        s1 = s1 + g * (resonatedInput - s1);
        s2 = s2 + g * (s1 - s2);
        s3 = s3 + g * (s2 - s3);
        s4 = s4 + g * (s3 - s4);

        // Clamp to avoid floating-point runaway at high resonance
        float result = s4;
        if (result > 1.0f) result = 1.0f;
        if (result < -1.0f) result = -1.0f;

        outBuffer[i] = result;
    }
}
