#pragma once

#include <imgui.h>
#include <string>

// ────────────────────────────────────────────────────────────────────────────
// ParamBox
// A rigid parameter display box with a label (left), key hint (right-anchored),
// and a dim cursor glyph + value on the bottom row.
// Read-only: visual feedback only, no click handling.
// ────────────────────────────────────────────────────────────────────────────
class ParamBox {
public:
    // Draw the box. label appears top-left, keyHint top-right,
    // value and unit appear as "> 123.45 unit" on the bottom row.
    void draw(const char* label, float value, const char* unit,
              const std::string& keyHint) const;
};