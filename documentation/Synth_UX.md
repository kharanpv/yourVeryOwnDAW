## 🎛️ Groovebox UX & Interaction Model: Synthesis Phase

This document outlines the implementation path for the synthesis debugging HUD. The core philosophy is **Dawless via Software**. The monitor acts as an empty canvas housing a scalable, floating HUD—mimicking the physical OLED screen of a hardware groovebox.

---

## 🛠️ Implementation Roadmap

### Phase 1: Input Abstraction & State Management

Before drawing graphics, we must capture and process raw keyboard data using time-based logic to emulate hardware encoders.

* [x] **Dynamic Keymap Router:** Create a mapping layer that connects raw `SDL_Keycode` inputs to logical engine actions (e.g., `ACTION_AUDITION_NOTE_1`, `ACTION_PARAM_UP`). This ensures the UI can dynamically display current keybindings and easily support custom user layouts in the future.
* [x] **Virtual Knob Acceleration:** Implement a state tracker that records how long an `ACTION_PARAM_UP` or `DOWN` key is held.
  * **Tap:** Increments/decrements by a micro-amount (Fine Tuning).
  * **Hold:** Applies an exponential multiplier to the rate of change the longer the key is depressed, allowing the user to "fly" to a target value quickly.
* [x] **The "Latch" Toggle:** Implement logic for a specific key (default: `Spacebar`) that suppresses `Note Off` messages. This allows a triggered note to drone endlessly so the user's hands are free to tweak parameters.
* [x] **Configuration Loader:** Parse a `settings.json` file on boot to dynamically load user keybindings and virtual knob tuning variables into the engine without recompiling.

### Phase 2: The UI Canvas & Render Loop
Before we can test our inputs or visualize audio, we must construct the empty room where the interface will live and establish the application's master infinite loop.
* **[See UI_Architecture.md for the complete build-out of this layer.](./UI_Architecture.md)**
* [x] **SDL2 Window Initialization:** Open a blank, hardware-accelerated desktop window.
* [x] **Dear ImGui Context:** Attach the ImGui rendering framework to the SDL window.
* [x] **The Master Loop:** Write the 60 FPS `while(running)` loop in `main.cpp` that keeps the application alive, polls for hardware inputs, and pushes pixels to the screen.

### Phase 3: The Auditory Feedback Loop (The Wiring)
With the UI canvas running, we translate our abstracted keyboard inputs into immediate DSP changes without causing thread locks.
* [x] **Live Parameter Injection:** When a parameter is actively being modified via the Virtual Knob logic, the new float value must be safely written to the Audio Thread (via `std::atomic` variables) so the user hears the filter sweep as the number changes on screen.
* [x] **The Audition Row:** Map the **Q W E R T Y U I O P [ ]** keys to a 12-note chromatic scale (C4–B4, MIDI 60–71). Pressing these immediately fires a `Note On` event to the `SynthVoice` to test frequency response.
* [x] **The Parameter Matrix UI:** Render a clean vertical list of available DSP parameters in ImGui. A `>` glyph precedes each value as a dim cursor. No highlighting is applied — every box is visually uniform.

> **Note:** Keybindings are user-reprogrammable via `config.json` at startup. A more inventive live reprogram system is planned for a future phase.

### Phase 4: Audition Keyboard Visualization
Now that the audition keys work and the UI is responsive, surface them visually so the user never has to guess which key maps to which note.
* [ ] **QWERTY Piano Overlay:** Render a 2-row keyboard diagram in the UI showing Q–P and [ ] with their MIDI note labels (C4–B4).
* [ ] **Active-Note Feedback:** Highlight the currently-playing note on the overlay.
* [ ] **Latch Sustain Indicator:** When latch is engaged, keep the held note highlighted on the overlay so the user sees exactly what is sustaining.

### Phase 5: Telemetry & The Data Bridge (The Oscilloscope)
To visualize the DSP, we need to get high-speed audio data back to the slow UI thread safely.
* [ ] **Thread-Safe Circular Buffer:** Allocate a fixed-size array. The high-priority Audio Thread writes every Nth generated audio sample into this buffer.
* [ ] **Buffer Synchronization:** Ensure the UI thread can safely read a snapshot of this buffer at 60 FPS without interrupting the Audio Thread's write cycle.
* [ ] **Oscilloscope Rendering:** Use `ImGui::PlotLines` fed by the Circular Buffer to draw a real-time representation of the synthesized waveform on our UI canvas.