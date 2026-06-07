#pragma once
#include <string>
#include <unordered_map>
#include <SDL.h> // We need SDL here to recognize SDL_Keycode
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ConfigLoader {
public:
    ConfigLoader(const std::string& configFilePath = "config.json");

    int getKnobHoldDelay() const;
    float getKnobAccelerationCurve() const;

    // We no longer return raw JSON. We return a clean, translated map of SDL Keycodes to Action Strings.
    std::unordered_map<SDL_Keycode, std::string> getKeybindings() const;

private:
    std::string filePath;
    json configData;

    void loadFile();
    void generateDefaultConfig();
};