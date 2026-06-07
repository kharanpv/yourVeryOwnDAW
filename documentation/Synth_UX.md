## 🎛️ Groovebox UX & Interaction Model: Synthesis Phase

This document outlines the implementation path for the synthesis debugging HUD. The core philosophy is **Dawless via Software**. The monitor acts as an empty canvas housing a scalable, floating HUD—mimicking the physical OLED screen of a hardware groovebox.

---

## 🛠️ Implementation Roadmap

### Phase 1: Input Abstraction & State Management

Before drawing graphics, we must capture and process raw keyboard data using time-based logic to emulate hardware encoders.

* [ ] **Dynamic Keymap Router:** Create a mapping layer that connects raw `SDL_Keycode` inputs to logical engine actions (e.g., `ACTION_AUDITION_NOTE_1`, `ACTION_PARAM_UP`). This ensures the UI can dynamically display current keybindings and easily support custom user layouts in the future.
* [ ] **Virtual Knob Acceleration:** Implement a state tracker that records how long an `ACTION_PARAM_UP` or `DOWN` key is held.
  * **Tap:** Increments/decrements by a micro-amount (Fine Tuning).
  * **Hold:** Applies an exponential multiplier to the rate of change the longer the key is depressed, allowing the user to "fly" to a target value quickly.
* [ ] **The "Latch" Toggle:** Implement logic for a specific key (default: `Spacebar`) that suppresses `Note Off` messages. This allows a triggered note to drone endlessly so the user's hands are free to tweak parameters.

### Phase 2: The Auditory Feedback Loop

The UI thread must translate the abstracted inputs into immediate DSP changes without causing thread locks.

* [ ] **Live Parameter Injection:** When a parameter is actively being modified via the Virtual Knob logic, the new float value must be safely written to the Audio Thread (via atomic variables or a lock-free queue) so the user hears the filter sweep as the number changes on screen.
* [ ] **The Audition Row:** Map the `Q` through `]` keys to a 12-note chromatic scale (C4 to B4). Pressing these immediately fires a `Note On` event to the `SynthVoice` to test frequency response at different pitches.

### Phase 3: Telemetry & The Data Bridge

To visualize the DSP, we need to get high-speed audio data back to the slow UI thread safely.

* [ ] **Thread-Safe Circular Buffer:** Allocate a fixed-size array. The high-priority Audio Thread writes every Nth generated audio sample into this buffer.
* [ ] **Buffer Synchronization:** Ensure the UI thread can safely read a snapshot of this buffer at 60 FPS without interrupting the Audio Thread's write cycle.

### Phase 4: HUD Rendering (Dear ImGui)

Constructing the visual "Hardware Screen" on the blank application canvas.

* [ ] **The Resizable Container:** Create a single Dear ImGui window (`ImGui::Begin`) configured with a dark, high-contrast theme. It must be user-scalable to fit different monitor sizes or testing scenarios.
* [ ] **The Oscilloscope:** Use `ImGui::PlotLines` fed by the Circular Buffer to draw a real-time representation of the synthesized waveform.
* [ ] **The Parameter Matrix:** Render a clean vertical list of available DSP parameters. Use a highly visible cursor (e.g., `>`) to indicate which parameter is currently targeted by the Virtual Knobs.
* [ ] **The Dynamic Keymap Legend:** A footer at the bottom of the HUD that polls the `Dynamic Keymap Router` and displays exactly which physical keys currently control the Virtual Knobs and Audition Row.