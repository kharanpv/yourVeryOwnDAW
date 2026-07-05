#include "ConfigLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>

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

    // The default config sets the same actions as the hardcoded defaults,
    // mapped to their SDL constant names.
    json kb = json::object();

    // -- Function / escape --
    kb["ESCAPE"]      = nullptr; kb["F1"]     = nullptr; kb["F2"]   = nullptr;
    kb["F3"]          = nullptr; kb["F4"]     = nullptr; kb["F5"]   = nullptr;
    kb["F6"]          = nullptr; kb["F7"]     = nullptr; kb["F8"]   = nullptr;
    kb["F9"]          = nullptr; kb["F10"]    = nullptr; kb["F11"]  = nullptr;
    kb["F12"]         = nullptr;

    // -- Number row --
    kb["BACKQUOTE"]   = nullptr;
    kb["1"]           = nullptr;
    kb["2"]           = nullptr;
    kb["3"]           = nullptr;
    kb["4"]           = nullptr;
    kb["5"]           = nullptr;
    kb["6"]           = "WAVEFORM_SINE";
    kb["7"]           = "WAVEFORM_SAW";
    kb["8"]           = "WAVEFORM_SQUARE";
    kb["9"]           = "WAVEFORM_TRIANGLE";
    kb["0"]           = "WAVEFORM_NOISE";
    kb["MINUS"]       = nullptr; kb["EQUALS"] = nullptr;
    kb["BACKSPACE"]   = nullptr;

    // -- QWERTY row --
    kb["TAB"]         = nullptr;
    kb["Q"]           = "AUDITION_NOTE_0";
    kb["W"]           = "AUDITION_NOTE_1";
    kb["E"]           = "AUDITION_NOTE_2";
    kb["R"]           = "AUDITION_NOTE_3";
    kb["T"]           = "AUDITION_NOTE_4";
    kb["Y"]           = "AUDITION_NOTE_5";
    kb["U"]           = "AUDITION_NOTE_6";
    kb["I"]           = "AUDITION_NOTE_7";
    kb["O"]           = "AUDITION_NOTE_8";
    kb["P"]           = "AUDITION_NOTE_9";
    kb["LEFTBRACKET"] = "AUDITION_NOTE_10";
    kb["RIGHTBRACKET"]= "AUDITION_NOTE_11";
    kb["BACKSLASH"]   = nullptr;

    // -- Home row + Caps --
    kb["CAPSLOCK"]    = nullptr;
    kb["A"]           = "CUTOFF_UP";
    kb["S"]           = "RES_UP";
    kb["D"]           = "ATTACK_UP";
    kb["F"]           = "HOLD_UP";
    kb["G"]           = "DECAY_UP";
    kb["H"]           = "SUSTAIN_UP";
    kb["J"]           = "RELEASE_UP";
    kb["K"]           = nullptr; kb["L"]      = nullptr;
    kb["SEMICOLON"]   = nullptr; kb["QUOTE"]  = nullptr;
    kb["RETURN"]      = nullptr;

    // -- Shift + bottom letter row --
    kb["LEFTSHIFT"]   = nullptr;
    kb["Z"]           = "CUTOFF_DOWN";
    kb["X"]           = "RES_DOWN";
    kb["C"]           = "ATTACK_DOWN";
    kb["V"]           = "HOLD_DOWN";
    kb["B"]           = "DECAY_DOWN";
    kb["N"]           = "SUSTAIN_DOWN";
    kb["M"]           = "RELEASE_DOWN";
    kb["COMMA"]       = nullptr; kb["PERIOD"] = nullptr; kb["SLASH"] = nullptr;
    kb["RIGHTSHIFT"]  = nullptr;

    // -- Modifier row --
    kb["LEFTCTRL"]    = nullptr; kb["LEFTGUI"]    = nullptr; kb["LEFTALT"] = nullptr;
    kb["SPACE"]       = "TOGGLE_LATCH";
    kb["RIGHTALT"]    = nullptr; kb["RIGHTGUI"]   = nullptr;
    kb["MENU"]        = nullptr; kb["RIGHTCTRL"]  = nullptr;

    // -- Navigation / edit --
    kb["PRINTSCREEN"] = nullptr; kb["SCROLLLOCK"] = nullptr; kb["PAUSE"] = nullptr;
    kb["INSERT"]      = nullptr; kb["HOME"]       = nullptr; kb["PAGEUP"] = nullptr;
    kb["DELETE"]      = nullptr; kb["END"]        = nullptr; kb["PAGEDOWN"] = nullptr;
    kb["UP"]          = nullptr; kb["DOWN"]       = nullptr;
    kb["LEFT"]        = nullptr; kb["RIGHT"]      = nullptr;

    // -- Numpad --
    kb["NUMLOCKCLEAR"]= nullptr;
    kb["KP_DIVIDE"]   = nullptr; kb["KP_MULTIPLY"] = nullptr; kb["KP_MINUS"] = nullptr;
    kb["KP_7"]        = nullptr; kb["KP_8"]        = nullptr; kb["KP_9"]    = nullptr;
    kb["KP_PLUS"]     = nullptr;
    kb["KP_4"]        = nullptr; kb["KP_5"]        = nullptr; kb["KP_6"]    = nullptr;
    kb["KP_1"]        = nullptr; kb["KP_2"]        = nullptr; kb["KP_3"]    = nullptr;
    kb["KP_0"]        = nullptr; kb["KP_PERIOD"]   = nullptr;
    kb["KP_ENTER"]    = nullptr;

    configData["keybindings"] = kb;

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

// ── THE DETERMINISTIC LOOKUP TABLE ──────────────────────────────────────────
// Maps SDL constant-name strings → SDL_Keycode values.
// No ambiguity: "6" is always number-row 6, "KP_6" is always numpad 6.
// The user writes the SDL constant name; the program uses this table.
// ────────────────────────────────────────────────────────────────────────────
static SDL_Keycode keyCodeFromName(const std::string& name) {
    // Single-character uppercase letters A–Z
    if (name.size() == 1) {
        char c = name[0];
        if (c >= 'A' && c <= 'Z') {
            // SDLK_a = SDLK_z are consecutive ASCII-lowercase in SDL2
            return static_cast<SDL_Keycode>(SDLK_a + (c - 'A'));
        }
        // Single digit 0–9
        if (c >= '0' && c <= '9') {
            return static_cast<SDL_Keycode>(SDLK_0 + (c - '0'));
        }
    }

    // Multi-character names — exhaustive map
    static const std::unordered_map<std::string, SDL_Keycode> table = {
        // ── Function / Escape ──
        {"ESCAPE",       SDLK_ESCAPE},
        {"F1",           SDLK_F1},
        {"F2",           SDLK_F2},
        {"F3",           SDLK_F3},
        {"F4",           SDLK_F4},
        {"F5",           SDLK_F5},
        {"F6",           SDLK_F6},
        {"F7",           SDLK_F7},
        {"F8",           SDLK_F8},
        {"F9",           SDLK_F9},
        {"F10",          SDLK_F10},
        {"F11",          SDLK_F11},
        {"F12",          SDLK_F12},

        // ── Punctuation / symbols (number row) ──
        {"BACKQUOTE",    SDLK_BACKQUOTE},
        {"MINUS",        SDLK_MINUS},
        {"EQUALS",       SDLK_EQUALS},
        {"BACKSPACE",    SDLK_BACKSPACE},

        // ── Whitespace / editing ──
        {"TAB",          SDLK_TAB},
        {"SPACE",        SDLK_SPACE},
        {"RETURN",       SDLK_RETURN},
        {"DELETE",       SDLK_DELETE},

        // ── Brackets / punctuation (QWERTY right side) ──
        {"LEFTBRACKET",  SDLK_LEFTBRACKET},
        {"RIGHTBRACKET", SDLK_RIGHTBRACKET},
        {"BACKSLASH",    SDLK_BACKSLASH},
        {"SEMICOLON",    SDLK_SEMICOLON},
        {"QUOTE",        SDLK_QUOTE},
        {"COMMA",        SDLK_COMMA},
        {"PERIOD",       SDLK_PERIOD},
        {"SLASH",        SDLK_SLASH},

        // ── Modifiers ──
        {"CAPSLOCK",     SDLK_CAPSLOCK},
        {"LEFTSHIFT",    SDLK_LSHIFT},
        {"RIGHTSHIFT",   SDLK_RSHIFT},
        {"LEFTCTRL",     SDLK_LCTRL},
        {"RIGHTCTRL",    SDLK_RCTRL},
        {"LEFTALT",      SDLK_LALT},
        {"RIGHTALT",     SDLK_RALT},
        {"LEFTGUI",      SDLK_LGUI},
        {"RIGHTGUI",     SDLK_RGUI},
        {"MENU",         SDLK_MENU},

        // ── Navigation / edit cluster ──
        {"PRINTSCREEN",  SDLK_PRINTSCREEN},
        {"SCROLLLOCK",   SDLK_SCROLLLOCK},
        {"PAUSE",        SDLK_PAUSE},
        {"INSERT",       SDLK_INSERT},
        {"HOME",         SDLK_HOME},
        {"PAGEUP",       SDLK_PAGEUP},
        {"END",          SDLK_END},
        {"PAGEDOWN",     SDLK_PAGEDOWN},
        {"UP",           SDLK_UP},
        {"DOWN",         SDLK_DOWN},
        {"LEFT",         SDLK_LEFT},
        {"RIGHT",        SDLK_RIGHT},

        // ── Numpad ──
        {"NUMLOCKCLEAR", SDLK_NUMLOCKCLEAR},
        {"KP_DIVIDE",    SDLK_KP_DIVIDE},
        {"KP_MULTIPLY",  SDLK_KP_MULTIPLY},
        {"KP_MINUS",     SDLK_KP_MINUS},
        {"KP_PLUS",      SDLK_KP_PLUS},
        {"KP_ENTER",     SDLK_KP_ENTER},
        {"KP_PERIOD",    SDLK_KP_PERIOD},
        {"KP_0",         SDLK_KP_0},
        {"KP_1",         SDLK_KP_1},
        {"KP_2",         SDLK_KP_2},
        {"KP_3",         SDLK_KP_3},
        {"KP_4",         SDLK_KP_4},
        {"KP_5",         SDLK_KP_5},
        {"KP_6",         SDLK_KP_6},
        {"KP_7",         SDLK_KP_7},
        {"KP_8",         SDLK_KP_8},
        {"KP_9",         SDLK_KP_9},
    };

    auto it = table.find(name);
    if (it != table.end()) return it->second;
    return SDLK_UNKNOWN;
}

// ── THE TRANSLATOR FUNCTION (now deterministic) ──
std::unordered_map<SDL_Keycode, std::string> ConfigLoader::getKeybindings() const {
    std::unordered_map<SDL_Keycode, std::string> mappedBindings;

    if (configData.contains("keybindings")) {
        for (auto& [keyName, actionValue] : configData["keybindings"].items()) {
            // Translate the SDL constant name → keycode
            SDL_Keycode keyCode = keyCodeFromName(keyName.c_str());

            if (keyCode == SDLK_UNKNOWN) {
                std::cerr << "Warning: Unknown key name in config.json ignored: "
                          << keyName << "\n";
                continue;
            }

            // null means explicitly unbound — skip (don't add to map)
            if (actionValue.is_null()) continue;

            mappedBindings[keyCode] = actionValue;
        }
    }

    return mappedBindings;
}