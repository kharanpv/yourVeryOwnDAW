#include "SynthDashboard.h"

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

        // --- 1. The Oscilloscope ---
        ImGui::BeginChild("Oscilloscope", ImVec2(-1, 200), true);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "LIVE WAVEFORM");
        
        // Safely extract the atomic data into a local array for ImGui to draw
        float localScope[SharedMatrix::SCOPE_SIZE];
        for (int i = 0; i < SharedMatrix::SCOPE_SIZE; ++i) {
            localScope[i] = dspMatrix->oscilloscopeBuffer[i].load(std::memory_order_relaxed);
        }
        
        // Draw the neon wave (Cyan)
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.8f, 1.0f));
        ImGui::PlotLines("##Scope", localScope, SharedMatrix::SCOPE_SIZE, 0, nullptr, -1.0f, 1.0f, ImVec2(-1, 150));
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

    // Query the router for the physical keys (e.g., returns "W" and "S")
    std::string keyUp = keyRouter->getKeyName(actionUp);
    std::string keyDown = keyRouter->getKeyName(actionDown);

    // Draw a blocky child window for the parameter
    ImGui::BeginChild(label, ImVec2(-1, 55), true, ImGuiWindowFlags_NoScrollbar);

    // Top Row: Label on the left, physical keys on the right
    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetWindowWidth() - 80); // Align to the far right
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[%s]/[%s]", keyUp.c_str(), keyDown.c_str());

    // Bottom Row: The actual numerical value
    // We use a bright Amber/Phosphor color for the data
    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "> %.2f %s", value, unit);

    ImGui::EndChild();
}

void SynthDashboard::drawToggleBox(const char* label, bool isActive, const std::string& stateStr,
                                   GrooveboxAction toggleAction) {

    std::string key = keyRouter->getKeyName(toggleAction);

    // If active, invert the background color of the box for high visual feedback
    if (isActive) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    }

    ImGui::BeginChild(label, ImVec2(-1, 55), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetWindowWidth() - 50);
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[%s]", key.c_str());

    // Bright color if active, dim if inactive
    ImVec4 textColor = isActive ? ImVec4(1.0f, 0.7f, 0.0f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImGui::TextColored(textColor, "> %s", stateStr.c_str());

    ImGui::EndChild();

    if (isActive) {
        ImGui::PopStyleColor(); // Restore normal background
    }
}
