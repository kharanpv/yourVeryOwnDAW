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
- [ ] **Subtractive Synthesis - Volume Envelope (ADSR):** A volume curve (Attack, Decay, Sustain, Release) so continuous tones have a distinct start and end.
- [ ] **Subtractive Synthesis - Further Shaping:** Filters and additional modulation sources (to be defined as development progresses).
- [ ] **Sampling:** Code to read `.wav` files into memory for drum playback and manipulation.
- [ ] **Voice Allocation / Object Pooling:** Pre-allocating a fixed massive amount of tracks/voices at startup to simulate "infinite" tracks without causing memory spikes during playback.
- [ ] **Backburner / Future Releases:** Frequency Modulation (FM) synthesis and Physical Modeling.

## 🕹️ 3. The Input / Routing Layer (The Nervous System)
This handles the controller data and bridges the human to the machine.
- [ ] **Hardware Polling:** An SDL2 loop that continuously reads the state of inputs (mouse, keyboard, gamepads).
- [ ] **State Translation:** Converting a raw input action into an actionable event (e.g., "Spacebar Pressed" = "Toggle Playback").
- [ ] **Routing Matrix:** Sending those events to either the UI (for navigation) or the Sequencer (for playback).

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
2. **Logic Update:** The Input Layer tells the **Sequencer** to activate Step 5 in the array.
3. **Audio Generation:** The **Audio Engine**'s callback fires. It checks the Sequencer, sees Step 5 is active, and tells the **DSP Engine** to generate a note.
4. **Output:** The Audio Engine sends that generated sound directly to the soundcard.
5. **Visual Feedback:** The **UI Thread** wakes up, checks the Sequencer's state, and draws a visual highlight on Step 5 on the screen.