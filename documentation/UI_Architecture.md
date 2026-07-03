# 🖥️ Groovebox UI & Application Architecture

## 🏛️ The Philosophy
The UI is not a complex web app; it is a **Hardware Diagnostic HUD**. It must be extremely lightweight, utilizing Dear ImGui to draw simple, high-contrast geometry over an SDL2 window. It runs strictly at 60 FPS on the Main Thread and communicates with the Audio Thread entirely through lock-free atomic variables.

## 🏗️ Architectural Foundations
To ensure the UI can scale beyond just a synthesizer (e.g., adding a drum machine or sequencer later), the graphical layer is strictly modular:

1. **The Application Window (`AppWindow`):** A generic wrapper around SDL2. It handles OS-level window management, DPI scaling, and the master rendering context. It knows nothing about music or DSP.
2. **The Render Context (Dear ImGui):** Attached to the `AppWindow`, ImGui handles the immediate-mode drawing of text, shapes, and layouts.
3. **The Thread Bridge (`SharedMatrix`):** A lock-free, `std::atomic` data structure. The main loop writes parameter changes here; the audio thread reads from it. This prevents audio dropouts.
4. **The Views (`SynthDashboard`):** Specific UI classes that are called during the render loop. They **read** the `SharedMatrix` atomics and tell ImGui what to draw — they never receive or process input directly.
5. **The Component Library (`source/ui/elements/`):** Reusable, self-contained UI widgets and pure-DSP generators. Every view composes these rather than duplicating drawing logic.

---

## 🛠️ Implementation Roadmap

### Step 1: The AppWindow Foundation
Build the generic OS window host that will eventually house all of our Groovebox screens.
* [x] **SDL2 Window Initialization:** Create an `AppWindow` class that successfully requests a hardware-accelerated window from the OS.
* [x] **Event Polling Setup:** Implement a clean method inside `AppWindow` to capture OS events (like window resizing or the user clicking the 'X' to quit).
* [x] **Render Clear & Swap:** Ensure the window can clear its background to a dark solid color and swap its buffers at a stable 60 FPS.

### Step 2: The UI Rendering Engine (Dear ImGui)
Integrate the immediate-mode GUI library to allow rapid interface development.
* [x] **CMake Integration:** Add Dear ImGui (and its SDL2 backend files) to the `CMakeLists.txt` build system.
* [x] **Context Initialization:** Initialize the ImGui context inside the `AppWindow` boot sequence.
* [x] **Proof of Life:** Successfully render `ImGui::ShowDemoWindow()` inside the main loop to verify fonts and hardware acceleration are working.

### Step 3: The Data Bridge
Establish the thread-safe connection between the upcoming UI and the existing DSP engine.
* [x] **The Shared Matrix:** Create a `struct` containing `std::atomic<float>` variables for all synth parameters (e.g., `filterCutoff`, `ampRelease`).
* [x] **Engine Wiring:** Pass a shared pointer (or reference) of this matrix into both the `SynthVoice` (Audio Thread) and the UI layer (Main Thread).

### Step 4: The Synth Dashboard (The View)
Build our first specific diagnostic view to visualize and control the subtractive synthesis engine.
* [x] **The Monolithic Canvas:** Create a single, borderless `ImGui::Begin` window, anchored and scaled exactly to the size of the OS viewport to emulate a hardware screen.
* [x] **The Telemetry Matrix:** Draw a clean, vertical list of rigid boxes displaying current DSP parameters by safely reading `std::atomic` values from the `SharedMatrix`.
* [x] **Inline Keymap Polling:** Dynamically query the `KeymapRouter` (`getKeyName`) inside the UI render loop so each parameter box automatically displays its assigned hardware hotkeys, eliminating the need for a global footer or UI cursors.
* [x] **AHDSR Envelope Graph:** Draw the dynamically calculated AHDSR envelope shape (attack, hold, decay, sustain, release) via the reusable `ScopeCanvas` component.

### Step 5: The Master Loop Execution
Tie the standalone modules together in `main.cpp` to boot the full application.
* [x] **Boot Sequence:** Initialize SDL, ImGui, the Audio Device, and the Input Router.
* [x] **The 60 FPS Loop:** Write the `while(running)` loop that sequentially polls inputs, updates the active Dashboard logic, and commands ImGui to render the frame.

### Step 6: Input Integration & Responsiveness
Bridge the gap between raw hardware inputs and visual/auditory updates.
* [x] **Live Input Wiring:** Physical keyboard presses are captured by the `InputStateManager`, which feeds the **main loop**. The main loop applies virtual knob acceleration logic and writes the resulting parameter values to the `SharedMatrix`. The UI only reads those atomics — it is never in the input path.
* [x] **Auditory Feedback Loop:** Parameter values written to the `SharedMatrix` by the main loop are immediately read by the audio thread, audibly manipulating the DSP engine in real-time. The UI reflects the same atomic values as passive confirmation.
* [x] **Immediate Tap Response:** Knob keys (A/Z, S/X, D/C) produce a one-shot delta on key-down — no 250ms dead zone. Tap gives immediate, perceptible parameter movement.
* [x] **Linear Acceleration Curve:** Replaced the quadratic `time² * 4.0` ramp with a linear growth curve (`baseSpeed * (1 + holdSeconds * accelCurve)`) for smooth, predictable hold-to-fly behavior.

### Step 7: Waveform View Redesign
Replaced the basic `ImGui::PlotLines` static cycle preview with a custom-drawn waveform plane that provides a 50ms time window of the computed (silent) oscillator output with full filter and envelope DSP applied.

* [x] **Waveform Plane Widget:** Custom `ScopeCanvas` in the right Telemetry column.
  - Pure black background (`#000000`).
  - Thin grey cross-hair axes through center (vertical + horizontal, `#505050`).
  - Y-axis labels: `1`, `0`, `-1` on the axis line.
  - X-axis labels: `0ms`, `25ms`, `50ms` at the bottom edge.
  - No rounding, no shadows, no gradients.

* [x] **2205-sample buffer:** Generated by running the oscillator + filter + envelope DSP at full 44.1kHz, then decimating by picking every 20th sample to produce 2205 samples for the 50ms window.

* [x] **Update-on-change:** The waveform is regenerated in `drawWaveformPlane()` every frame. Parameter changes are reflected immediately.

* [x] **Preview frequency source:** The waveform uses the frequency of the last-played MIDI note. Falls back to 220Hz (A3) when no note has been played yet.

* [x] **Frequency Reference Box:** In the left Parameters column, a read-only box labelled "OSC FREQ" displays the current preview frequency in Hz.

### Step 8: Audition Key Visualization (The Piano Roll Overlay)
Now that the engine is proven responsive, make the audition keyboard visible in the UI so the user can see which keys map to which notes without alt-tabbing to a reference.

* [ ] **Piano Roll Widget:** Render a piano keyboard in the right Telemetry column, below the waveform plane and envelope graph. The widget spans the full width of the column and the overall window height is expanded to accommodate it.

  **Visual style (80s/90s terminal HUD):**
  - White keys are bright white (`#FFFFFF`) with thin black (`#000000`) borders between them.
  - Black keys are pitch black (`#000000`) with thin white (`#FFFFFF`) borders for contrast.
  - The background behind the keyboard is the standard void black (`#0D0D0D`).
  - No rounding, no shadows, no gradients — flat, blocky, terminal-renderer aesthetic.
  - Key width: calculated dynamically so that the full octave fits the available width.
  - Black key height: approximately 60% of white key height, overhanging the white key boundary.

* [ ] **Key Labels (QWERTY binding):** Displayed **inside the key, at the bottom**, in dim grey text (`#595959`). For C4 (Q key), the `Q` appears near the bottom of the white key. For C#4 (W key), the `W` appears near the bottom of the black key.

* [ ] **Key Labels (MIDI note name + frequency):** Displayed **inside the key, at the top**, in dim grey text (`#595959`). Two lines: the MIDI note name (e.g., `C4`) on line 1, and the frequency in Hz (e.g., `261.63`) on line 2.

* [ ] **Active-Note Indicator (no latch):** When a note is sounding and latch is off:
  - White key active: fill becomes darker (a mid-grey, approximately `#808080`).
  - Black key active: fill becomes lighter (a dark grey, approximately `#404040`).
  - The two greys must be visually distinguishable from each other.

* [ ] **Latch Indicator:** When latch is engaged and a note is held:
  - The same visual as the active-note indicator, but persistent — the key remains greyed even after the user releases the keyboard key.
  - No separate accent color; the latch state is conveyed purely by the persistence of the active-note visual.

### Step 9: UI Refactor — Reusable Component Library
Extracted all visual elements into standalone, reusable components under `source/ui/elements/` to support future views.

* [x] **TerminalStyle.h:** Centralized color palette, sizing, and thickness constants.
* [x] **ScopeCanvas.h/.cpp:** Reusable graph widget (black bg, grey axes, data→pixel transform, plotLine, labels). Used by both waveform plane and envelope graph.
* [x] **ParamBox.h/.cpp:** Generic read-only parameter display box.
* [x] **ToggleBox.h/.cpp:** Generic on/off state box with active background inversion.
* [x] **WaveformSelector.h/.cpp:** Vertical waveform list with active highlight.
* [x] **WaveformGenerator.h/.cpp:** Pure DSP (oscillator → filter → envelope), no ImGui dependency.
* [x] **EnvelopeGenerator.h/.cpp:** Pure math AHDSR shape generator, no ImGui dependency.
* [x] **SynthDashboard refactored:** Now a thin composer that reads SharedMatrix and calls the above components.

### Step 10: Full AHDSR Parameter Controls
Wire up keybindable controls for all five AHDSR envelope stages in the left Parameters column.

* [ ] **AMP HOLD box:** Add keybindable up/down controls for the Hold duration.
* [ ] **AMP DECAY box:** Add keybindable up/down controls for the Decay time.
* [ ] **AMP SUSTAIN box:** Add keybindable up/down controls for the Sustain level.
* [ ] **AMP RELEASE box:** Add keybindable up/down controls for the Release time.
* [ ] **Define key mappings** in `config.json` and `KeymapRouter` for the four new knob actions.

### Step 11: Piano Roll Overlay (The Audition Keyboard)
Render a visual piano keyboard in the right Telemetry column so the user can see which QWERTY keys map to which notes.

* [ ] **PianoRollWidget:** New reusable component under `source/ui/elements/`.
* [ ] **Visual style (80s/90s terminal HUD):**
  - White keys: bright white (`#FFFFFF`) with thin black (`#000000`) borders.
  - Black keys: pitch black (`#000000`) with thin white (`#FFFFFF`) borders.
  - No rounding, no shadows, no gradients.
  - Black key height: ~60% of white key height, overhanging the white key boundary.
* [ ] **Key Labels (QWERTY binding):** Displayed inside the key at the bottom in dim grey (`#595959`).
* [ ] **Key Labels (MIDI note + frequency):** Displayed inside the key at the top in dim grey (`#595959`). Two lines: note name then frequency in Hz.
* [ ] **Active-Note Indicator:** White key active → darker (`#808080`). Black key active → lighter (`#404040`). Must be distinguishable.
* [ ] **Latch Indicator:** Same visual as active-note, but persistent.

### Step 12: Future UI Modules (The Expansion Pack)
Because the graphical layer relies purely on passing data through the `SharedMatrix`, we can indefinitely build and swap out new views without touching the core engine.
* [ ] **The Grid Sequencer View:** A visual timeline for plotting notes and automation.
* [ ] **The Drum Machine / Sampler View:** An interface to load, slice, and trigger `.wav` files.
* [ ] **The Master Mixer View:** A central hub showing volume levels, panning, and global effects across all active voices.