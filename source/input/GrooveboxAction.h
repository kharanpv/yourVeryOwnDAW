#pragma once

enum class GrooveboxAction {
    NONE,

    // --- Direct Parameter Controls (No Navigation) ---
    CUTOFF_UP,
    CUTOFF_DOWN,
    RES_UP,
    RES_DOWN,
    ATTACK_UP,
    ATTACK_DOWN,

    // --- State / Transport ---
    TOGGLE_LATCH,

    // --- Audition Row (Chromatic Octave) ---
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