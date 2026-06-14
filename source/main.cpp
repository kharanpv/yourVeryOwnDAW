#include <iostream>
#include <memory>
#include <SDL.h> // Required for fetching keyboard state

// --- Core & Audio Headers ---
#include "core/SharedMatrix.h"
#include "core/AudioDevice.h" 
#include "dsp/SynthVoice.h"
#include "dsp/oscillators/SawOscillator.h"

// --- Input Headers ---
#include "input/KeymapRouter.h"
#include "input/InputStateManager.h"

// --- UI Headers ---
#include "ui/AppWindow.h"
#include "ui/SynthDashboard.h"

int main(int argc, char* argv[]) {
    std::cout << "Booting Open Source DAW..." << std::endl;

    // =========================================================
    // 1. ALLOCATE THE THREAD BRIDGE (The Single Source of Truth)
    // =========================================================
    auto sharedMatrix = std::make_shared<SharedMatrix>();

    // =========================================================
    // 2. BOOT THE INPUT ENGINE
    // =========================================================
    auto keyRouter = std::make_shared<KeymapRouter>();
    
    // CORRECTED: InputStateManager takes no arguments! 
    // It is a pure math/state tracker.
    InputStateManager inputManager;

    // (Optional) Tweak your knob feel right here on boot!
    inputManager.setBaseSpeed(0.005f);
    inputManager.setAccelerationCurve(4.0f);

    // =========================================================
    // 3. BOOT THE AUDIO ENGINE
    // =========================================================
    SawOscillator defaultOsc(44100.0f, 440.0f);
    SynthVoice myVoice(&defaultOsc, sharedMatrix);

    AudioDevice audioDev;
    if (!audioDev.initialize(&myVoice)) {
        std::cerr << "Failed to open soundcard!" << std::endl;
        return -1;
    }
    audioDev.start(); // Audio thread is now running in the background!

    // =========================================================
    // 4. BOOT THE VISUAL ENGINE (UI Canvas)
    // =========================================================
    AppWindow appWindow;
    if (!appWindow.initialize("Groovebox Diagnostic HUD", 1024, 768)) {
        std::cerr << "Failed to boot UI canvas. Exiting." << std::endl;
        return -1;
    }

    // Instantiate the Dashboard, injecting the data dependencies
    SynthDashboard dashboard(sharedMatrix, keyRouter);

    std::cout << "Engine fully booted. Entering main loop." << std::endl;

    // =========================================================
    // 5. THE MASTER LOOP (60 FPS Main Thread)
    // =========================================================
    while (appWindow.isRunning()) {

        // 1. Drain OS Events
        appWindow.processEvents();

        // 2. CORRECTED: Advance the physics of the Virtual Knob using Time (Ticks)
        float knobDelta = inputManager.updateContinuous(SDL_GetTicks());
        
        if (knobDelta != 0.0f) {
            // FUTURE WIRING: This is where we will eventually say:
            // if (activeCursor == CUTOFF) sharedMatrix->filterCutoff += knobDelta;
        }

        // 3. Start the immediate-mode UI frame
        appWindow.beginUiFrame();

        // 4. Render our Synth Dashboard
        dashboard.render();

        // 5. Wipe the screen & push ImGui to the physical monitor
        appWindow.clear();
        appWindow.drawUi();
        appWindow.present();
    }

    // =========================================================
    // 6. CLEAN SHUTDOWN
    // =========================================================
    std::cout << "Shutting down engine safely." << std::endl;
    audioDev.stop(); // Safely kill audio thread first to prevent mid-buffer pops
    appWindow.shutdown(); // Then kill graphics and SDL

    return 0;
}