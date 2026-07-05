#pragma once

#include <imgui.h>

// ────────────────────────────────────────────────────────────────────────────
// TerminalStyle
// Central style constants for the 80s/90s terminal HUD aesthetic.
// Every UI element should pull from here so the look stays consistent.
// ────────────────────────────────────────────────────────────────────────────
namespace TerminalStyle {

// ── Colors (ImU32 for ImDrawList) ──
inline ImU32 colBlack()     { return IM_COL32(0,   0,   0,   255); }
inline ImU32 colWhite()     { return IM_COL32(255, 255, 255, 255); }
inline ImU32 colVoidBg()    { return IM_COL32(13,  13,  13,  255); } // #0D0D0D
inline ImU32 colBorder()    { return IM_COL32(77,  77,  77,  255); } // #4D4D4D
inline ImU32 colAxis()      { return IM_COL32(80,  80,  80,  255); } // #505050
inline ImU32 colDimText()   { return IM_COL32(100, 100, 100, 255); } // #646464
inline ImU32 colHintText()  { return IM_COL32(89,  89,  89,  255); } // #595959
inline ImU32 colAmber()   { return IM_COL32(255, 178, 0,   255); } // #FFB200
inline ImU32 colActiveBg()  { return IM_COL32(51,  51,  51,  255); } // #333333

// ── Colors (ImVec4 for ImGui text/colored items) ──
inline ImVec4 textWhite()   { return ImVec4(0.90f, 0.90f, 0.90f, 1.00f); }
inline ImVec4 textDim()     { return ImVec4(0.50f, 0.50f, 0.50f, 1.00f); }
inline ImVec4 textHint()    { return ImVec4(0.35f, 0.35f, 0.35f, 1.00f); }
inline ImVec4 textLabel()   { return ImVec4(0.40f, 0.40f, 0.40f, 1.00f); }
inline ImVec4 accent()      { return ImVec4(0.0f,  1.0f,  0.8f,  1.00f); }
inline ImVec4 magenta()     { return ImVec4(1.0f,  0.2f,  0.8f,  1.00f); }
inline ImVec4 amber()       { return ImVec4(1.0f,  0.7f,  0.0f,  1.00f); }
inline ImVec4 bgBlack()     { return ImVec4(0.00f, 0.00f, 0.00f, 1.00f); }
inline ImVec4 childBg()     { return ImVec4(0.05f, 0.05f, 0.05f, 1.00f); }
inline ImVec4 activeChildBg() { return ImVec4(0.20f, 0.20f, 0.20f, 1.00f); }

// ── Sizing ──
inline float paramBoxHeight()  { return 83.0f; }
inline float selectorItemH()   { return 42.0f; }
inline float windowPadding()   { return 8.0f;  }
inline float lineThickness()   { return 2.5f;  }
inline float axisThickness()   { return 1.0f;  }

// ── ScopeCanvas margins (space outside the graph for labels + ticks) ──
inline float labelMargin()     { return 40.0f; }  // uniform margin on all four sides
inline float tickLength()      { return 6.0f;  }

} // namespace TerminalStyle
