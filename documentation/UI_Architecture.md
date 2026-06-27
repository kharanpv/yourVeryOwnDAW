# 🖥️ Groovebox UI & Application Architecture

## 🏛️ The Philosophy
The UI is not a complex web app; it is a **Hardware Diagnostic HUD**. It must be extremely lightweight, utilizing Dear ImGui to draw simple, high-contrast geometry over an SDL2 window. It runs strictly at 60 FPS on the Main Thread and communicates with the Audio Thread entirely through lock-free atomic variables.

## 🏗️ Architectural Foundations
To ensure the UI can scale beyond just a synthesizer (e.g., adding a drum machine or sequencer later), the graphical layer is strictly modular:

1. **The Application Window (`AppWindow`):** A generic wrapper around SDL2. It handles OS-level window management, DPI scaling, and the master rendering context. It knows nothing about music or DSP.
2. **The Render Context (Dear ImGui):** Attached to the `AppWindow`, ImGui handles the immediate-mode drawing of text, shapes, and layouts.
3. **The Thread Bridge (`SharedMatrix`):** A lock-free, `std::atomic` data structure. The main loop writes parameter changes here; the audio thread reads from it. This prevents audio dropouts.
4. **The Views (`SynthDashboard`):** Specific UI classes that are called during the render loop. They **read** the `SharedMatrix` atomics and tell ImGui what to draw — they never receive or process input directly.

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
* [x] **Audio Visualizers:** Draw the real-time oscilloscope and dynamically calculated ADSR envelope shape in the telemetry column using `ImGui::PlotLines`.

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

### Step 7: Audition Key Visualization
Now that the engine is proven responsive, make the 12-note audition keyboard visible in the UI so the user can see which keys map to which notes without alt-tabbing to a reference.
* [ ] **Keyboard Overlay Widget:** Draw a 1- or 2-row QWERTY piano layout in the right Telemetry column, highlighting the keys Q–P and [ ] and labelling them with their MIDI note names (C4–B4).
* [ ] **Active-Note Indicator:** Light up the pressed key on the overlay when a note is sounding.
* [ ] **Latch State Indicator:** If latch is engaged, show a persistent sustain indicator on the overlay so the user can see which note is being held.

### Step 8: Visual Design Overhaul
Now that the UI is functional and responsive, replace the basic prototype layout with the final polished aesthetic.
* [ ] **Design Language:** Settle on a cohesive hardware-style design language (colors, padding, telemetry grouping, and typography).
* [ ] **Layout Refactoring:** Revise the initial ImGui layout blockiness to implement the new design language, improving readability and the overall user experience.

### Step 9: Future UI Modules (The Expansion Pack)
Because the graphical layer relies purely on passing data through the `SharedMatrix`, we can indefinitely build and swap out new views without touching the core engine.
* [ ] **The Grid Sequencer View:** A visual timeline for plotting notes and automation.
* [ ] **The Drum Machine / Sampler View:** An interface to load, slice, and trigger `.wav` files.
* [ ] **The Master Mixer View:** A central hub showing volume levels, panning, and global effects across all active voices.