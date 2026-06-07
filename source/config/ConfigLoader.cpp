#include "ConfigLoader.h"
#include <fstream>
#include <iostream>

ConfigLoader::ConfigLoader(const std::string& configFilePath) : filePath(configFilePath) {
    loadFile();
}

void ConfigLoader::loadFile() {
    std::ifstream file(filePath);
    
    if (file.is_open()) {
        try {
            file >> configData;
            std::cout << "Loaded " << filePath << " successfully.\n";
        } catch (json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << "\nFalling back to defaults.\n";
            generateDefaultConfig();
        }
    } else {
        std::cout << filePath << " not found. Generating default configuration...\n";
        generateDefaultConfig();
    }
}

void ConfigLoader::generateDefaultConfig() {
    configData["hardware_emulation"]["knob_hold_delay_ms"] = 300;
    configData["hardware_emulation"]["knob_acceleration_curve"] = 4.0f;
    
    // Human-readable defaults (SDL expects standard Title Case or Uppercase)
    configData["keybindings"]["Up"] = "NAV_UP";
    configData["keybindings"]["Down"] = "NAV_DOWN";
    configData["keybindings"]["Q"] = "AUDITION_NOTE_1";
    configData["keybindings"]["Space"] = "TOGGLE_LATCH";

    std::ofstream file(filePath);
    if (file.is_open()) {
        file << configData.dump(4);
        file.close();
    }
}

int ConfigLoader::getKnobHoldDelay() const {
    return configData["hardware_emulation"].value("knob_hold_delay_ms", 300);
}

float ConfigLoader::getKnobAccelerationCurve() const {
    return configData["hardware_emulation"].value("knob_acceleration_curve", 4.0f);
}

// THE TRANSLATOR FUNCTION
std::unordered_map<SDL_Keycode, std::string> ConfigLoader::getKeybindings() const {
    std::unordered_map<SDL_Keycode, std::string> mappedBindings;
    
    if (configData.contains("keybindings")) {
        for (auto& [keyName, actionString] : configData["keybindings"].items()) {
            
            // Ask SDL to convert the string (e.g., "Up") into its actual hardware integer
            SDL_Keycode keyCode = SDL_GetKeyFromName(keyName.c_str());
            
            // If the user typed a valid key, add it to our map
            if (keyCode != SDLK_UNKNOWN) {
                mappedBindings[keyCode] = actionString;
            } else {
                // If the user made a typo in the config.json, don't crash, just warn them!
                std::cerr << "Warning: Unknown key name in config.json ignored: " << keyName << "\n";
            }
        }
    }
    return mappedBindings;
}