#pragma once

#include <imgui.h>
#include <string>

// ────────────────────────────────────────────────────────────────────────────
// ToggleBox
// A box that displays an on/off state. When active, the child background
// inverts to a lighter grey. The state string is shown with the cursor glyph.
// ────────────────────────────────────────────────────────────────────────────
class ToggleBox {
public:
    void draw(const char* label, bool isActive, const std::string& stateStr,
              const std::string& keyHint) const;
};