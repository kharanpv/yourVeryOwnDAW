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

    /// Load bindings from a config.json-derived map of key-name → action-name strings.
    /// Overrides defaults: each action gets a new key assignment from config.json.
    /// Any action *not* mentioned in the config keeps its default binding.
    /// Unrecognized action names are silently skipped.
    void loadFromConfig(const std::unordered_map<SDL_Keycode, std::string>& bindings);

    /// Remove any existing binding for the given action.
    /// Used internally by loadFromConfig to avoid stale key->action entries.
    void unmapAction(GrooveboxAction action);

private:
    std::unordered_map<SDL_Keycode, GrooveboxAction> activeKeymap;

    /// Parse a human-readable action string (e.g. "CUTOFF_UP") into a GrooveboxAction.
    /// Returns GrooveboxAction::NONE if the string does not match any known action.
    static GrooveboxAction actionFromString(const std::string& name);
};