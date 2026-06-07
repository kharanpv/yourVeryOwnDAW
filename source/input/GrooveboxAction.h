#pragma once

enum class GrooveboxAction {
    NONE, // Default state for unmapped keys

    // --- HUD Navigation ---
    // Moves the active cursor up and down the Parameter Matrix[cite: 95].
    NAV_UP,
    NAV_DOWN,

    // --- Parameter Editing ("Virtual Knob") ---
    // Modifies whichever parameter the cursor is currently grabbing.
    PARAM_INC, // Twist right
    PARAM_DEC, // Twist left

    // --- State / Transport ---
    // Toggles whether note-offs are suppressed[cite: 106].
    TOGGLE_LATCH,

    // --- Audition Row (Chromatic Octave) ---
    // Triggers the synth engine to test frequency response[cite: 104].
    AUDITION_NOTE_0,  // C4
    AUDITION_NOTE_1,  // C#4
    AUDITION_NOTE_2,  // D4
    AUDITION_NOTE_3,  // D#4
    AUDITION_NOTE_4,  // E4
    AUDITION_NOTE_5,  // F4
    AUDITION_NOTE_6,  // F#4
    AUDITION_NOTE_7,  // G4
    AUDITION_NOTE_8,  // G#4
    AUDITION_NOTE_9,  // A4
    AUDITION_NOTE_10, // A#4
    AUDITION_NOTE_11  // B4
};