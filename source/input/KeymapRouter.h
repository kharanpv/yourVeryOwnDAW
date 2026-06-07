#pragma once

#include <unordered_map>
#include <SDL2/SDL.h> // Adjust this path if your build uses <SDL.h>
#include "GrooveboxAction.h"

class KeymapRouter {
public:
    KeymapRouter();
    ~KeymapRouter() = default;

    // Populates the map with our initial layout design
    void loadLayerOneMapping();

    // The primary query function for the event loop
    GrooveboxAction getAction(SDL_Keycode key) const;

    // Future-proofing: Allows custom user mappings to be injected later
    void mapKey(SDL_Keycode key, GrooveboxAction action);

private:
    std::unordered_map<SDL_Keycode, GrooveboxAction> activeKeymap;
};