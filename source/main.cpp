#include <iostream>
#include <memory>
#include <algorithm> // for std::max/min
#include <SDL.h>     

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

    // 1. ALLOCATE THE THREAD BRIDGE (The Spreadsheet)
    auto sharedMatrix = std::make_shared<SharedMatrix>();

    // 2. BOOT THE INPUT ENGINE
    auto keyRouter = std::make_shared<KeymapRouter>();
    InputStateManager inputManager;
    inputManager.setBaseSpeed(0.005f);
    inputManager.setAccelerationCurve(4.0f);

    // 3. BOOT THE AUDIO ENGINE
    SawOscillator defaultOsc(44100.0f, 440.0f);
    
    // NEW: We pass '0' to assign this voice to Track 0 in the Matrix!
    SynthVoice myVoice(&defaultOsc, sharedMatrix, 0); 

    AudioDevice audioDev;
    if (!audioDev.initialize(&myVoice)) {
        std::cerr << "Failed to open soundcard!" << std::endl;
        return -1;
    }
    audioDev.start();

    // 4. BOOT THE VISUAL ENGINE (UI Canvas)
    AppWindow appWindow;
    if (!appWindow.initialize("Groovebox Diagnostic HUD", 1024, 768)) {
        std::cerr << "Failed to boot UI canvas. Exiting." << std::endl;
        return -1;
    }

    SynthDashboard dashboard(sharedMatrix, keyRouter);

    std::cout << "Engine fully booted. Entering main loop." << std::endl;

    // 5. THE MASTER LOOP (60 FPS Main Thread)
    while (appWindow.isRunning()) {

        // A. THE ORCHESTRATOR: Route OS Events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            
            // Feed the UI
            appWindow.processImGuiEvent(&event);

            // Handle Window Close
            if (event.type == SDL_QUIT) {
                appWindow.requestQuit(); 
            }

            // Feed the Input State Manager
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                if (event.key.repeat == 0) {
                    bool isDown = (event.type == SDL_KEYDOWN);
                    GrooveboxAction action = keyRouter->getAction(event.key.keysym.sym);
                    inputManager.processEvent(action, isDown);
                }
            }
        }

        // B. Update Virtual Knobs
        float knobDelta = inputManager.updateContinuous(SDL_GetTicks());
        if (knobDelta != 0.0f) {
            // Test the knob wiring safely on the Master Volume!
            float currentVol = sharedMatrix->masterVolume.load();
            float newVol = std::max(0.0f, std::min(1.0f, currentVol + knobDelta));
            sharedMatrix->masterVolume.store(newVol);
        }

        // C. Render UI
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