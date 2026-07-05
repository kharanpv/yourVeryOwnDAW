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
    
    // Human-readable defaults
    configData["keybindings"]["A"]      = "CUTOFF_UP";
    configData["keybindings"]["Z"]      = "CUTOFF_DOWN";
    configData["keybindings"]["S"]      = "RES_UP";
    configData["keybindings"]["X"]      = "RES_DOWN";
    configData["keybindings"]["D"]      = "ATTACK_UP";
    configData["keybindings"]["C"]      = "ATTACK_DOWN";
    configData["keybindings"]["F"]      = "HOLD_UP";
    configData["keybindings"]["V"]      = "HOLD_DOWN";
    configData["keybindings"]["G"]      = "DECAY_UP";
    configData["keybindings"]["B"]      = "DECAY_DOWN";
    configData["keybindings"]["H"]      = "SUSTAIN_UP";
    configData["keybindings"]["N"]      = "SUSTAIN_DOWN";
    configData["keybindings"]["J"]      = "RELEASE_UP";
    configData["keybindings"]["M"]      = "RELEASE_DOWN";
    configData["keybindings"]["Space"]  = "TOGGLE_LATCH";
    configData["keybindings"]["1"]      = "WAVEFORM_SINE";
    configData["keybindings"]["2"]      = "WAVEFORM_SAW";
    configData["keybindings"]["3"]      = "WAVEFORM_SQUARE";
    configData["keybindings"]["4"]      = "WAVEFORM_TRIANGLE";
    configData["keybindings"]["5"]      = "WAVEFORM_NOISE";
    configData["keybindings"]["Q"]      = "AUDITION_NOTE_0";
    configData["keybindings"]["W"]      = "AUDITION_NOTE_1";
    configData["keybindings"]["E"]      = "AUDITION_NOTE_2";
    configData["keybindings"]["R"]      = "AUDITION_NOTE_3";
    configData["keybindings"]["T"]      = "AUDITION_NOTE_4";
    configData["keybindings"]["Y"]      = "AUDITION_NOTE_5";
    configData["keybindings"]["U"]      = "AUDITION_NOTE_6";
    configData["keybindings"]["I"]      = "AUDITION_NOTE_7";
    configData["keybindings"]["O"]      = "AUDITION_NOTE_8";
    configData["keybindings"]["P"]      = "AUDITION_NOTE_9";
    configData["keybindings"]["["]      = "AUDITION_NOTE_10";
    configData["keybindings"]["\"]"]    = "AUDITION_NOTE_11";

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