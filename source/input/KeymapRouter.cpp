#include "KeymapRouter.h"

KeymapRouter::KeymapRouter() {
    // Automatically load the default bindings on startup
    loadLayerOneMapping();
}

void KeymapRouter::loadLayerOneMapping() {
    activeKeymap.clear();

    // 1. Direct Parameter Control (Top row = UP, Bottom row = DOWN)
    activeKeymap[SDLK_a] = GrooveboxAction::CUTOFF_UP;
    activeKeymap[SDLK_z] = GrooveboxAction::CUTOFF_DOWN;
    
    activeKeymap[SDLK_s] = GrooveboxAction::RES_UP;
    activeKeymap[SDLK_x] = GrooveboxAction::RES_DOWN;
    
    activeKeymap[SDLK_d] = GrooveboxAction::ATTACK_UP;
    activeKeymap[SDLK_c] = GrooveboxAction::ATTACK_DOWN;

    activeKeymap[SDLK_f] = GrooveboxAction::HOLD_UP;
    activeKeymap[SDLK_v] = GrooveboxAction::HOLD_DOWN;

    activeKeymap[SDLK_g] = GrooveboxAction::DECAY_UP;
    activeKeymap[SDLK_b] = GrooveboxAction::DECAY_DOWN;

    activeKeymap[SDLK_h] = GrooveboxAction::SUSTAIN_UP;
    activeKeymap[SDLK_n] = GrooveboxAction::SUSTAIN_DOWN;

    activeKeymap[SDLK_j] = GrooveboxAction::RELEASE_UP;
    activeKeymap[SDLK_m] = GrooveboxAction::RELEASE_DOWN;

    // 2. State/Latch
    activeKeymap[SDLK_SPACE] = GrooveboxAction::TOGGLE_LATCH;

    // 3. Waveform Selection — number keys 1-5
    activeKeymap[SDLK_1] = GrooveboxAction::WAVEFORM_SINE;
    activeKeymap[SDLK_2] = GrooveboxAction::WAVEFORM_SAW;
    activeKeymap[SDLK_3] = GrooveboxAction::WAVEFORM_SQUARE;
    activeKeymap[SDLK_4] = GrooveboxAction::WAVEFORM_TRIANGLE;
    activeKeymap[SDLK_5] = GrooveboxAction::WAVEFORM_NOISE;

    // 4. Audition Row — 12-key chromatic octave mapped across the QWERTY top row + brackets
    //    Q=C4(60)  W=C#4(61) E=D4(62)  R=D#4(63) T=E4(64)  Y=F4(65)
    //    U=F#4(66) I=G4(67)  O=G#4(68) P=A4(69)  [=A#4(70) ]=B4(71)
    activeKeymap[SDLK_q]            = GrooveboxAction::AUDITION_NOTE_0;  // C4  (MIDI 60)
    activeKeymap[SDLK_w]            = GrooveboxAction::AUDITION_NOTE_1;  // C#4 (MIDI 61)
    activeKeymap[SDLK_e]            = GrooveboxAction::AUDITION_NOTE_2;  // D4  (MIDI 62)
    activeKeymap[SDLK_r]            = GrooveboxAction::AUDITION_NOTE_3;  // D#4 (MIDI 63)
    activeKeymap[SDLK_t]            = GrooveboxAction::AUDITION_NOTE_4;  // E4  (MIDI 64)
    activeKeymap[SDLK_y]            = GrooveboxAction::AUDITION_NOTE_5;  // F4  (MIDI 65)
    activeKeymap[SDLK_u]            = GrooveboxAction::AUDITION_NOTE_6;  // F#4 (MIDI 66)
    activeKeymap[SDLK_i]            = GrooveboxAction::AUDITION_NOTE_7;  // G4  (MIDI 67)
    activeKeymap[SDLK_o]            = GrooveboxAction::AUDITION_NOTE_8;  // G#4 (MIDI 68)
    activeKeymap[SDLK_p]            = GrooveboxAction::AUDITION_NOTE_9;  // A4  (MIDI 69)
    activeKeymap[SDLK_LEFTBRACKET]  = GrooveboxAction::AUDITION_NOTE_10; // A#4 (MIDI 70)
    activeKeymap[SDLK_RIGHTBRACKET] = GrooveboxAction::AUDITION_NOTE_11; // B4  (MIDI 71)
}

GrooveboxAction KeymapRouter::getAction(SDL_Keycode key) const {
    auto it = activeKeymap.find(key);
    
    // If the key is found in our map, return the assigned action
    if (it != activeKeymap.end()) {
        return it->second;
    }
    
    // If the user pressed an unmapped key (e.g., 'Z' or 'Enter'), safely do nothing
    return GrooveboxAction::NONE;
}

void KeymapRouter::mapKey(SDL_Keycode key, GrooveboxAction action) {
    activeKeymap[key] = action;
}

std::string KeymapRouter::getKeyName(GrooveboxAction action) const {
    // Search the map for the action
    for (const auto& pair : activeKeymap) {
        if (pair.second == action) {
            // SDL_GetKeyName returns a clean string like "W", "Space", or "Up"
            return SDL_GetKeyName(pair.first); 
        }
    }
    return "UNMAPPED"; // If the action has no key assigned
}