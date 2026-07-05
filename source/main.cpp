#include <iostream>
#include <memory>
#include <SDL.h>

// --- Core & Audio Headers ---
#include "core/SharedMatrix.h"
#include "core/AudioDevice.h"
#include "dsp/SynthVoice.h"
#include "dsp/oscillators/SineOscillator.h"
#include "dsp/oscillators/SawOscillator.h"
#include "dsp/oscillators/SquareOscillator.h"
#include "dsp/oscillators/TriangleOscillator.h"
#include "dsp/oscillators/NoiseGenerator.h"

// --- Input Headers ---
#include "input/KeymapRouter.h"
#include "input/InputStateManager.h"

// --- UI Headers ---
#include "ui/AppWindow.h"
#include "ui/SynthDashboard.h"

// ---------------------------------------------------------------
// Per-parameter ranges: defines how each knob delta is applied.
// The raw delta from InputStateManager is a normalized float; we
// scale it by `range` and clamp the result to [min, max].
// ---------------------------------------------------------------
struct ParamRange {
    ParamID id;
    float   min;
    float   max;
    float   range; // Multiply raw delta by this to get Hz/seconds/etc.
};

// Each knob action maps to one entry in this table.
// Order must match the GrooveboxAction -> ParamID routing below.
static const ParamRange PARAM_RANGES[] = {
    { P_FILTER_CUTOFF, 20.0f,   20000.0f, 300.0f  }, // Hz — sweeps fast
    { P_FILTER_RES,    0.0f,    1.0f,     1.0f    }, // 0–1 normalized
    { P_AMP_ATTACK,    0.0f,    10.0f,    1.0f    }, // seconds
    { P_AMP_HOLD,      0.0f,    10.0f,    1.0f    }, // seconds
    { P_AMP_DECAY,     0.0f,    10.0f,    1.0f    }, // seconds
    { P_AMP_SUSTAIN,   0.0f,     1.0f,    1.0f    }, // 0-1 normalized level
    { P_AMP_RELEASE,   0.0f,    10.0f,    1.0f    }, // seconds
};

// Clamp helper
static float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// MIDI note index (0–11) → MIDI note number (C4=60 through B4=71)
static const int AUDITION_MIDI_NOTES[12] = {
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71
};

int main(int argc, char* argv[]) {
    std::cout << "Booting Open Source DAW..." << std::endl;

    // 1. ALLOCATE THE THREAD BRIDGE
    auto sharedMatrix = std::make_shared<SharedMatrix>();

    // 2. BOOT THE INPUT ENGINE
    auto keyRouter = std::make_shared<KeymapRouter>();
    InputStateManager inputManager;
    inputManager.setBaseSpeed(0.005f);
    inputManager.setAccelerationCurve(4.0f);

    // 3. BOOT THE AUDIO ENGINE — create all 5 waveforms
    SineOscillator     sineOsc(44100.0f, 440.0f);
    SawOscillator      sawOsc(44100.0f, 440.0f);
    SquareOscillator   squareOsc(44100.0f, 440.0f);
    TriangleOscillator triOsc(44100.0f, 440.0f);
    NoiseGenerator     noiseOsc(0.1f);

    // Start with the waveform already stored in the matrix (default = Saw, index 1)
    Oscillator* oscillators[5] = { &sineOsc, &sawOsc, &squareOsc, &triOsc, &noiseOsc };
    int currentWaveIdx = (int)sharedMatrix->tracks[0].params[P_OSC_WAVEFORM].load();
    if (currentWaveIdx < 0 || currentWaveIdx > 4) currentWaveIdx = 1;
    SynthVoice myVoice(oscillators[currentWaveIdx], sharedMatrix, 0);

    AudioDevice audioDev;
    if (!audioDev.initialize(&myVoice)) {
        std::cerr << "Failed to open soundcard!" << std::endl;
        return -1;
    }
    audioDev.start();

    // 4. BOOT THE VISUAL ENGINE
    AppWindow appWindow;
    if (!appWindow.initialize("Groovebox Diagnostic HUD", 960, 660)) {
        std::cerr << "Failed to boot UI canvas. Exiting." << std::endl;
        return -1;
    }

    SynthDashboard dashboard(sharedMatrix, keyRouter);

    std::cout << "Engine fully booted. Entering main loop." << std::endl;

    // 5. THE MASTER LOOP (60 FPS Main Thread)
    while (appWindow.isRunning()) {

        // -------------------------------------------------------
        // A. POLL OS EVENTS
        // -------------------------------------------------------
        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            appWindow.processImGuiEvent(&event);

            if (event.type == SDL_QUIT) {
                appWindow.requestQuit();
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                // Ignore key-repeat events — only process true down/up edges.
                if (event.key.repeat == 0) {
                    bool isDown = (event.type == SDL_KEYDOWN);
                    GrooveboxAction action = keyRouter->getAction(event.key.keysym.sym);
                    inputManager.processEvent(action, isDown);
                }
            }
        }

        // -------------------------------------------------------
        // B. DRAIN EDGE EVENTS (latch toggle, audition notes)
        //    These are one-shot events fired on key-down/up edges.
        // -------------------------------------------------------

        // Latch toggle — write the new boolean into the matrix
        if (inputManager.consumeLatchToggle()) {
            bool latched = inputManager.getIsLatched();
            sharedMatrix->tracks[0].params[P_LATCH_MODE].store(latched ? 1.0f : 0.0f);
        }

        // Note-on edge — tune oscillator to MIDI note, store frequency, and trigger envelope
        int noteOn = inputManager.consumeNoteOn();
        if (noteOn >= 0 && noteOn < 12) {
            float freq = 440.0f * std::pow(2.0f, (AUDITION_MIDI_NOTES[noteOn] - 69) / 12.0f);
            sharedMatrix->previewFreq.store(freq);
            myVoice.playNote(AUDITION_MIDI_NOTES[noteOn]);
        }

        // Note-off edge — release envelope (latch suppression is inside SynthVoice)
        int noteOff = inputManager.consumeNoteOff();
        if (noteOff >= 0 && noteOff < 12) {
            myVoice.releaseNote();
        }

        // Waveform switching — consume edge events and update oscillator + matrix
        int waveAction = inputManager.consumeWaveformChange();
        if (waveAction >= 0 && waveAction <= 4) {
            sharedMatrix->tracks[0].params[P_OSC_WAVEFORM].store((float)waveAction);
            myVoice.setOscillator(oscillators[waveAction]);
        }

        // -------------------------------------------------------
        // C. VIRTUAL KNOB — apply continuous delta to the targeted
        //    parameter and write it atomically to the matrix.
        // -------------------------------------------------------
        float rawDelta = inputManager.updateContinuous(SDL_GetTicks());
        if (rawDelta != 0.0f) {
            int paramIdx = inputManager.getActiveParamIndex();
            if (paramIdx >= 0) {
                const ParamRange& pr = PARAM_RANGES[paramIdx];
                float current = sharedMatrix->tracks[0].params[pr.id].load();
                float next = clampf(current + rawDelta * pr.range, pr.min, pr.max);
                sharedMatrix->tracks[0].params[pr.id].store(next);
            }
        }

        // -------------------------------------------------------
        // D. RENDER UI
        // The dashboard is a pure HUD — it only reads SharedMatrix,
        // never writes. All control flow is in this main loop only.
        // -------------------------------------------------------
        appWindow.beginUiFrame();
        dashboard.render();
        appWindow.clear();
        appWindow.drawUi();
        appWindow.present();
    }

    // 6. CLEAN SHUTDOWN
    std::cout << "Shutting down engine safely." << std::endl;
    audioDev.stop();
    appWindow.shutdown();

    return 0;
}
