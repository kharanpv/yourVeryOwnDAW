#pragma once

#include <imgui.h>
#include <string>

// ────────────────────────────────────────────────────────────────────────────
// WaveformSelector
// A vertical list of waveform names with an active highlight.
// The currently-selected waveform is shown in the accent color.
// Each row right-anchors its keyboard shortcut hint.
// ────────────────────────────────────────────────────────────────────────────
class WaveformSelector {
public:
    static constexpr int NUM_WAVEFORMS = 5;
    static constexpr const char* NAMES[NUM_WAVEFORMS] = {
        "SINE", "SAW", "SQUARE", "TRIANGLE", "NOISE"
    };

    // Draw the selector. keyHints[i] is the key binding for waveform i.
    void draw(int currentWave, const char* keyHints[NUM_WAVEFORMS]) const;

    // Overload: accept std::string array
    void draw(int currentWave, const std::string keyHints[NUM_WAVEFORMS]) const;
};