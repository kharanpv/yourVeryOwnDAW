#pragma once
#include <atomic>

// 1. THE PARAMETER DICTIONARY (The Columns)
enum ParamID {
    // --- Global/Oscillator ---
    P_OSC_WAVEFORM = 0,
    P_LATCH_MODE,

    // --- Amp Envelopes ---
    P_AMP_PREDELAY,
    P_AMP_ATTACK,
    P_AMP_HOLD,
    P_AMP_DECAY,
    P_AMP_SUSTAIN,
    P_AMP_RELEASE,
    
    // --- Filter Parameters ---
    P_FILTER_CUTOFF,
    P_FILTER_RES,
    P_FILTER_ENV_AMT,

    // --- Filter Envelopes ---
    P_FILTER_PREDELAY,
    P_FILTER_ATTACK,
    P_FILTER_HOLD,
    P_FILTER_DECAY,
    P_FILTER_SUSTAIN,
    P_FILTER_RELEASE,
    
    // Auto-counts the total size!
    P_MAX_PARAMS 
};

// 2. THE TRACK STATE (The Rows)
struct TrackState {
    std::atomic<bool> isActive{ false };
    std::atomic<int> instrumentType{ 1 }; // 1 = Subtractive Synth

    // The Flat Grid of Parameters
    std::atomic<float> params[P_MAX_PARAMS]; 

    TrackState() {
        for (int i = 0; i < P_MAX_PARAMS; ++i) {
            params[i] = 0.0f;
        }
        // Set sensible defaults so the synth makes sound immediately
        params[P_OSC_WAVEFORM] = 1.0f;     
        params[P_FILTER_CUTOFF] = 1000.0f;
        params[P_FILTER_RES] = 0.1f;
        params[P_FILTER_ENV_AMT] = 2000.0f;
        params[P_AMP_SUSTAIN] = 0.8f;
        params[P_AMP_RELEASE] = 0.5f;
    }
};

// 3. THE SHARED MATRIX (The Lock-Free Bridge)
struct SharedMatrix {
    std::atomic<float> masterVolume{ 0.8f };

    static constexpr int MAX_TRACKS = 64; // Your Absolute Maximum Limit
    TrackState tracks[MAX_TRACKS];

    static constexpr int SCOPE_SIZE = 512;
    std::atomic<float> oscilloscopeBuffer[SCOPE_SIZE];
    std::atomic<int> scopeWriteIndex{ 0 };

    // Frequency tracking for the waveform preview oscilloscope.
    // Updated by the main loop when a note is played; read by the dashboard.
    // Falls back to 220.0f (A3) as the default when no note has been played.
    std::atomic<float> previewFreq{ 220.0f };
};