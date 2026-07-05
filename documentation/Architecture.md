# 🎛️ Groovebox System Architecture & Component Tracker

## 🏛️ The Golden Rule
**The UI Thread and the Audio Thread must NEVER wait for each other.** Visuals may drop frames or lag, but the audio callback must process continuously to prevent clicks and pops.

---

## 🧩 1. The Audio Engine (The Brain)
This is the lowest-level layer. It talks directly to the OS and the soundcard. It runs on a high-priority, dedicated thread.
- [x] **Audio Device Initialization:** Code to open the computer's soundcard using SDL2.
- [x] **The Audio Callback Function:** The high-priority loop that constantly fills the soundcard's buffers with audio data exactly when requested.
- [x] **Buffer Management:** Ensuring the flow of audio data never starves or overflows.

## 🔊 2. The Sound Engine (DSP & Synthesis)
This lives inside the Audio Engine. It is the code that mathematically generates, shapes, and alters the audio signal.
- [x] **Fundamental Oscillator Library:** Base `Oscillator` class and standard basic waveforms (Sine, Sawtooth, Square, Triangle, and White Noise).
- [x] **Amplitude & Delay Shaping (AHDSR):** A volume curve (Attack, Hold, Decay, Sustain, Release) plus a Pre-Delay timer so continuous tones have a precise, dynamic start and end.
- [x] **Subtractive Synthesis - Pure 4-Pole Filter & Envelope:** A digital 4-pole low-pass filter cascade, modulated by its own independent AHDSR envelope to smoothly curve the wave's sharp digital jumps over time.
- [x] **Filter Resonance (Feedback Loop):** Implementing a mathematical feedback path from the 4th filter stage back to the input to amplify the cutoff frequency and add analog "squelch/whistle."
- [x] **Synth Parameter UX (The Debug/Test Environment):** Establishing a robust interactive graphical interface (Dear ImGui) mapped to hardware inputs. This provides a real-time testing ground to dial in envelopes and filters before writing more complex DSP.
  [See Synth_UX.md for the complete build-out of this layer.](./Synth_UX.md)
- [ ] **Low Frequency Oscillators (LFO):** Reusing core oscillator math at sub-audio rates (0.1Hz - 20Hz) to continuously modulate pitch, volume, or filter cutoff for evolving, moving sounds.
- [ ] **Sampling Engine:** Memory allocation and bit-parsing to load `.wav` files into RAM for polyphonic drum playback and pitch manipulation.
- [ ] **Voice Allocation / Object Pooling:** Pre-allocating a fixed, massive pool of synthesizer and sampler voices at startup to simulate "infinite" track counts without triggering memory allocation faults on the audio thread.
- [ ] **Backburner / Future Releases:** Frequency Modulation (FM) synthesis, Pitch Glide (Portamento), and Physical Modeling.

## 🕹️ 3. The Input / Routing Layer (The Nervous System)
This handles raw physical inputs, abstracting them into musical actions, and safely writing them to the Audio Thread's memory.
- [x] Hardware Polling: An SDL2 loop on the Main Thread that cleanly captures OS events (keyboard, mouse, window management).
- [x] Dynamic Keymap Router: An abstraction layer translating raw physical keys into logical `GrooveboxAction`s (e.g., Arrow Up -> ACTION_PARAM_UP).
- [x] Virtual Encoders (State Manager): Time-based tracking that converts held keys into exponential floating-point deltas to simulate the "feel" of hardware knobs.
- [x] The Lock-Free Data Grid (SharedMatrix): A pre-allocated, fixed-size 2D array (Tracks × Parameters) of `std::atomic<float>` variables. The main loop writes parameter changes here; the Audio thread reads them. The UI thread also reads (never writes) these atomics to display current state — completely eliminating memory allocation and thread locks during playback.

> **Note:** Keybindings are user-reprogrammable via `config.json` at the project root. On boot, `ConfigLoader` reads the file and passes every binding to `KeymapRouter::loadFromConfig()`, which overlays them on top of the hardcoded defaults. Any action not present in `config.json` keeps its fallback binding. The UI dynamically queries the router for all key hints — no hardcoded key names exist outside the fallback defaults. A more inventive live reprogram system is planned for a future phase.

## ⏱️ 4. The Sequencer / Clock (The Heartbeat)
This keeps the music perfectly in time and tells the DSP Engine what to play and when.
- [ ] **Master Transport:** Manages the global state (Play, Stop) and the global BPM (Beats Per Minute).
- [ ] **16-Step Grid Array (MVP):** A fixed data array tracking which steps in the current sequence are active.
- [ ] **Playhead Tracking:** Calculating exactly which step is currently playing based on the audio buffer size and sample rate.

## 🖥️ 5. The Lightweight UI (The Screen)
The graphical overlay drawn using Dear ImGui. It runs on the main thread and simply observes what the audio/sequencer threads are doing.
- [ ] **Render Initialization:** Setting up Dear ImGui to draw over the SDL2 window.
- [ ] **The Grid Display:** Visualizing the 16-step sequencer array so the user knows what is programmed.
- [ ] **Playhead / Cursor Visualizer:** Highlighting the current step playing and the user's current cursor location.

---

## 🔄 How the Components Interact (Data Flow)

1. **Input:** The user presses an input control. The **Input Layer** detects this.
2. **Logic Update:** The Input Layer tells the **main loop**, which writes the change to the `SharedMatrix` atomics (e.g., updating a live synth parameter). For sequencer actions the main loop also tells the **Sequencer** to activate Step 5 in the array.
3. **Audio Generation:** The **Audio Engine**'s callback fires. It checks the Sequencer, sees Step 5 is active, and tells the **DSP Engine** to generate a note.
4. **Output:** The Audio Engine sends that generated sound directly to the soundcard.
5. **Visual Feedback:** The **UI Thread** wakes up, checks the Sequencer's state, and draws a visual highlight on Step 5 on the screen.