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

    // --- Audition Row: 12 keys Q W E R T Y U I O P [ ] => full chromatic octave C4–B4 ---
    // Q=C4(60)  W=C#4(61) E=D4(62)  R=D#4(63) T=E4(64)  Y=F4(65)
    // U=F#4(66) I=G4(67)  O=G#4(68) P=A4(69)  [=A#4(70) ]=B4(71)
    AUDITION_NOTE_0,  // Q  -> C4  (MIDI 60)
    AUDITION_NOTE_1,  // W  -> C#4 (MIDI 61)
    AUDITION_NOTE_2,  // E  -> D4  (MIDI 62)
    AUDITION_NOTE_3,  // R  -> D#4 (MIDI 63)
    AUDITION_NOTE_4,  // T  -> E4  (MIDI 64)
    AUDITION_NOTE_5,  // Y  -> F4  (MIDI 65)
    AUDITION_NOTE_6,  // U  -> F#4 (MIDI 66)
    AUDITION_NOTE_7,  // I  -> G4  (MIDI 67)
    AUDITION_NOTE_8,  // O  -> G#4 (MIDI 68)
    AUDITION_NOTE_9,  // P  -> A4  (MIDI 69)
    AUDITION_NOTE_10, // [  -> A#4 (MIDI 70)
    AUDITION_NOTE_11  // ]  -> B4  (MIDI 71)
};