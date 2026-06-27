#pragma once

#include <memory>
#include <string>
#include <imgui.h>
#include "../core/SharedMatrix.h"    // Your atomic data bridge
#include "../input/KeymapRouter.h"    // To look up dynamic key bindings

class SynthDashboard {
public:
    SynthDashboard(std::shared_ptr<SharedMatrix> matrix, std::shared_ptr<KeymapRouter> router);

    // Called every frame in your main loop
    void render();

private:
    std::shared_ptr<SharedMatrix> dspMatrix;
    std::shared_ptr<KeymapRouter> keyRouter;

    // --- High-Resolution Terminal Aesthetic Helpers ---
    void setupTerminalTheme();

    // Draws a rigid box for continuous values (e.g., Cutoff, Attack).
    // No active-highlighting — every box looks the same regardless of last-touched.
    void drawContinuousBox(const char* label, float value, const char* unit,
                           GrooveboxAction actionUp, GrooveboxAction actionDown);

    // Draws a rigid box for booleans (e.g., Latch, Oscillator Type)
    void drawToggleBox(const char* label, bool isActive, const std::string& stateStr, 
                       GrooveboxAction toggleAction);
};