#include "KeymapRouter.h"

KeymapRouter::KeymapRouter() {
    // Automatically load the default bindings on startup
    loadLayerOneMapping();
}

GrooveboxAction KeymapRouter::actionFromString(const std::string& name) {
    // Build the lookup once — static so it persists across calls
    static const std::unordered_map<std::string, GrooveboxAction> table = {
        {"CUTOFF_UP",       GrooveboxAction::CUTOFF_UP},
        {"CUTOFF_DOWN",     GrooveboxAction::CUTOFF_DOWN},
        {"RES_UP",          GrooveboxAction::RES_UP},
        {"RES_DOWN",        GrooveboxAction::RES_DOWN},
        {"ATTACK_UP",       GrooveboxAction::ATTACK_UP},
        {"ATTACK_DOWN",     GrooveboxAction::ATTACK_DOWN},
        {"HOLD_UP",         GrooveboxAction::HOLD_UP},
        {"HOLD_DOWN",       GrooveboxAction::HOLD_DOWN},
        {"DECAY_UP",        GrooveboxAction::DECAY_UP},
        {"DECAY_DOWN",      GrooveboxAction::DECAY_DOWN},
        {"SUSTAIN_UP",      GrooveboxAction::SUSTAIN_UP},
        {"SUSTAIN_DOWN",    GrooveboxAction::SUSTAIN_DOWN},
        {"RELEASE_UP",      GrooveboxAction::RELEASE_UP},
        {"RELEASE_DOWN",    GrooveboxAction::RELEASE_DOWN},
        {"TOGGLE_LATCH",    GrooveboxAction::TOGGLE_LATCH},
        {"WAVEFORM_SINE",   GrooveboxAction::WAVEFORM_SINE},
        {"WAVEFORM_SAW",    GrooveboxAction::WAVEFORM_SAW},
        {"WAVEFORM_SQUARE", GrooveboxAction::WAVEFORM_SQUARE},
        {"WAVEFORM_TRIANGLE", GrooveboxAction::WAVEFORM_TRIANGLE},
        {"WAVEFORM_NOISE",  GrooveboxAction::WAVEFORM_NOISE},
        {"AUDITION_NOTE_0", GrooveboxAction::AUDITION_NOTE_0},
        {"AUDITION_NOTE_1", GrooveboxAction::AUDITION_NOTE_1},
        {"AUDITION_NOTE_2", GrooveboxAction::AUDITION_NOTE_2},
        {"AUDITION_NOTE_3", GrooveboxAction::AUDITION_NOTE_3},
        {"AUDITION_NOTE_4", GrooveboxAction::AUDITION_NOTE_4},
        {"AUDITION_NOTE_5", GrooveboxAction::AUDITION_NOTE_5},
        {"AUDITION_NOTE_6", GrooveboxAction::AUDITION_NOTE_6},
        {"AUDITION_NOTE_7", GrooveboxAction::AUDITION_NOTE_7},
        {"AUDITION_NOTE_8", GrooveboxAction::AUDITION_NOTE_8},
        {"AUDITION_NOTE_9", GrooveboxAction::AUDITION_NOTE_9},
        {"AUDITION_NOTE_10", GrooveboxAction::AUDITION_NOTE_10},
        {"AUDITION_NOTE_11", GrooveboxAction::AUDITION_NOTE_11},
    };

    auto it = table.find(name);
    if (it != table.end()) return it->second;
    return GrooveboxAction::NONE;
}

void KeymapRouter::unmapAction(GrooveboxAction action) {
    // Remove any key that currently maps to this action
    for (auto it = activeKeymap.begin(); it != activeKeymap.end(); ) {
        if (it->second == action) {
            it = activeKeymap.erase(it);
        } else {
            ++it;
        }
    }
}

void KeymapRouter::loadFromConfig(const std::unordered_map<SDL_Keycode, std::string>& bindings) {
    // First unmap any default bindings for actions that the user is overriding.
    // This prevents stale key->action entries (e.g. "1" still bound to WAVEFORM_SINE
    // after the user remaps it to "6").
    for (const auto& [keyCode, actionName] : bindings) {
        GrooveboxAction action = actionFromString(actionName);
        if (action != GrooveboxAction::NONE) {
            unmapAction(action);
        }
    }
    // Now overlay the user's explicit bindings
    for (const auto& [keyCode, actionName] : bindings) {
        GrooveboxAction action = actionFromString(actionName);
        if (action != GrooveboxAction::NONE) {
            activeKeymap[keyCode] = action;
        }
    }
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