# 🖥️ Groovebox UI & Application Architecture

## 🏛️ The Philosophy
The UI is not a complex web app; it is a **Hardware Diagnostic HUD**. It must be extremely lightweight, utilizing Dear ImGui to draw simple, high-contrast geometry over an SDL2 window. It runs strictly at 60 FPS on the Main Thread and communicates with the Audio Thread entirely through lock-free atomic variables.

## 🏗️ Architectural Foundations
To ensure the UI can scale beyond just a synthesizer (e.g., adding a drum machine or sequencer later), the graphical layer is strictly modular:

1. **The Application Window (`AppWindow`):** A generic wrapper around SDL2. It handles OS-level window management, DPI scaling, and the master rendering context. It knows nothing about music or DSP.
2. **The Render Context (Dear ImGui):** Attached to the `AppWindow`, ImGui handles the immediate-mode drawing of text, shapes, and layouts.
3. **The Thread Bridge (`SharedMatrix`):** A lock-free, `std::atomic` data structure. The UI thread writes parameter changes here; the audio thread reads from it. This prevents audio dropouts.
4. **The Views (`SynthDashboard`):** Specific UI classes that are called during the render loop. They read user input, update the `SharedMatrix`, and tell ImGui what to draw.

---

## 🛠️ Implementation Roadmap

### Step 1: The AppWindow Foundation
Build the generic OS window host that will eventually house all of our Groovebox screens.
* [x] **SDL2 Window Initialization:** Create an `AppWindow` class that successfully requests a hardware-accelerated window from the OS.
* [x] **Event Polling Setup:** Implement a clean method inside `AppWindow` to capture OS events (like window resizing or the user clicking the 'X' to quit).
* [x] **Render Clear & Swap:** Ensure the window can clear its background to a dark solid color and swap its buffers at a stable 60 FPS.

### Step 2: The UI Rendering Engine (Dear ImGui)
Integrate the immediate-mode GUI library to allow rapid interface development.
* [ ] **CMake Integration:** Add Dear ImGui (and its SDL2 backend files) to the `CMakeLists.txt` build system.
* [ ] **Context Initialization:** Initialize the ImGui context inside the `AppWindow` boot sequence.
* [ ] **Proof of Life:** Successfully render `ImGui::ShowDemoWindow()` inside the main loop to verify fonts and hardware acceleration are working.

### Step 3: The Data Bridge
Establish the thread-safe connection between the upcoming UI and the existing DSP engine.
* [ ] **The Shared Matrix:** Create a `struct` containing `std::atomic<float>` variables for all synth parameters (e.g., `filterCutoff`, `ampRelease`).
* [ ] **Engine Wiring:** Pass a shared pointer (or reference) of this matrix into both the `SynthVoice` (Audio Thread) and the UI layer (Main Thread).

### Step 4: The Synth Dashboard (The View)
Build our first specific diagnostic view to visualize and control the subtractive synthesis engine.
* [ ] **The Resizable Container:** Create a single `ImGui::Begin` window, anchored and scaled to the size of the `AppWindow`.
* [ ] **Parameter Matrix Rendering:** Draw a clean, vertical list of the current DSP parameters by reading them from the `SharedMatrix`.
* [ ] **Virtual Knob Hookup:** Connect the `InputStateManager` to the ImGui display. Ensure that when a user holds an arrow key, the UI correctly displays the accelerated value changes.
* [ ] **The Dynamic Keymap Legend:** Render a footer at the bottom of the HUD that polls the `KeymapRouter` to display the active controls (e.g., `[Space] Latch | [A-K] Audition`).

### Step 5: The Master Loop Execution
Tie the standalone modules together in `main.cpp` to boot the full application.
* [ ] **Boot Sequence:** Initialize SDL, ImGui, the Audio Device, and the Input Router.
* [ ] **The 60 FPS Loop:** Write the `while(running)` loop that sequentially polls inputs, updates the active Dashboard logic, and commands ImGui to render the frame.
