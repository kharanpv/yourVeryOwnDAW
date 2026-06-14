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

    // 2. State/Latch
    activeKeymap[SDLK_SPACE] = GrooveboxAction::TOGGLE_LATCH;

    // 3. Audition Row (Q through ] mapping to C4 through B4)
    activeKeymap[SDLK_q]            = GrooveboxAction::AUDITION_NOTE_0;
    activeKeymap[SDLK_w]            = GrooveboxAction::AUDITION_NOTE_1;
    activeKeymap[SDLK_e]            = GrooveboxAction::AUDITION_NOTE_2;
    activeKeymap[SDLK_r]            = GrooveboxAction::AUDITION_NOTE_3;
    activeKeymap[SDLK_t]            = GrooveboxAction::AUDITION_NOTE_4;
    activeKeymap[SDLK_y]            = GrooveboxAction::AUDITION_NOTE_5;
    activeKeymap[SDLK_u]            = GrooveboxAction::AUDITION_NOTE_6;
    activeKeymap[SDLK_i]            = GrooveboxAction::AUDITION_NOTE_7;
    activeKeymap[SDLK_o]            = GrooveboxAction::AUDITION_NOTE_8;
    activeKeymap[SDLK_p]            = GrooveboxAction::AUDITION_NOTE_9;
    activeKeymap[SDLK_LEFTBRACKET]  = GrooveboxAction::AUDITION_NOTE_10; 
    activeKeymap[SDLK_RIGHTBRACKET] = GrooveboxAction::AUDITION_NOTE_11; 
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