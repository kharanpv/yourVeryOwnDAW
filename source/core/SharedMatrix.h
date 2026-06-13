#pragma once

#include <atomic>

// This struct is the single source of truth for the Groovebox's current state.
// The UI Thread WRITES to it. The Audio Thread READS from it.
struct SharedMatrix {
    
    // --- Global / Input State ---
    std::atomic<bool> isLatched{ false };

    // --- Oscillator Parameters ---
    // e.g., 0 = Sine, 1 = Saw, 2 = Square, 3 = Triangle, 4 = Noise
    std::atomic<float> oscWaveform{ 1.0f }; 

    // --- Filter Parameters (Core) ---
    std::atomic<float> filterCutoff{ 1000.0f }; // Base cutoff in Hz
    std::atomic<float> filterResonance{ 0.1f }; // Q / Feedback amount
    std::atomic<float> filterEnvAmount{ 2000.0f }; // How high the envelope pushes the cutoff

    // --- Amp AHDSR Envelope ---
    std::atomic<float> ampPreDelay{ 0.0f };
    std::atomic<float> ampAttack{ 0.01f };
    std::atomic<float> ampHold{ 0.0f };
    std::atomic<float> ampDecay{ 0.1f };
    std::atomic<float> ampSustain{ 0.8f };
    std::atomic<float> ampRelease{ 0.5f };

    // --- Filter AHDSR Envelope ---
    std::atomic<float> filterPreDelay{ 0.0f };
    std::atomic<float> filterAttack{ 0.01f };
    std::atomic<float> filterHold{ 0.0f };
    std::atomic<float> filterDecay{ 0.1f };
    std::atomic<float> filterSustain{ 0.5f };
    std::atomic<float> filterRelease{ 0.5f };
};