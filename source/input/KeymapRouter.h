#pragma once

#include <unordered_map>
#include <string>           
#include <SDL2/SDL.h>
#include "GrooveboxAction.h"

class KeymapRouter {
public:
    KeymapRouter();
    ~KeymapRouter() = default;

    void loadLayerOneMapping();
    GrooveboxAction getAction(SDL_Keycode key) const;
    void mapKey(SDL_Keycode key, GrooveboxAction action);
    std::string getKeyName(GrooveboxAction action) const;

private:
    std::unordered_map<SDL_Keycode, GrooveboxAction> activeKeymap;
};