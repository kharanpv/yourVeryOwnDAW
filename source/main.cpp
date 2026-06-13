#include <iostream>
#include "ui/AppWindow.h"
#include "imgui.h" // We include this here just to call ShowDemoWindow()

int main(int argc, char* argv[]) {
    std::cout << "Booting Open Source DAW..." << std::endl;

    AppWindow appWindow;
    if (!appWindow.initialize("Groovebox Diagnostic HUD", 1024, 768)) {
        std::cerr << "Failed to boot UI canvas. Exiting." << std::endl;
        return -1;
    }

    std::cout << "UI Canvas established. Entering main loop." << std::endl;

    // The Master Render Loop (60 FPS)
    while (appWindow.isRunning()) {
        
        // 1. Drain OS Events & Feed them to ImGui
        appWindow.processEvents();

        // 2. Start the immediate-mode UI frame
        appWindow.beginUiFrame();

        // 3. Define the UI (This disappears every frame!)
        ImGui::ShowDemoWindow(); 

        // 4. Wipe the hardware screen
        appWindow.clear();

        // 5. Draw the UI geometry over the wiped screen
        appWindow.drawUi();

        // 6. Push the pixels to the monitor
        appWindow.present();
    }

    std::cout << "Shutting down engine safely." << std::endl;
    appWindow.shutdown();

    return 0;
}

// #define SDL_MAIN_HANDLED

// #include "core/AudioDevice.h"
// #include "dsp/oscillators/SineOscillator.h"
// #include "dsp/oscillators/SquareOscillator.h"
// #include "dsp/oscillators/SawOscillator.h"
// #include "dsp/oscillators/TriangleOscillator.h"
// #include "dsp/oscillators/NoiseGenerator.h"
// #include "dsp/SynthVoice.h" 

// #include <SDL2/SDL.h>
// #include <iostream>
// #include <thread>
// #include <atomic>

// // Cycles through 4 distinct synthesizer configurations.
// // Runs continuously until the user advances to the next oscillator via standard input.
// void runOscillatorTestSequence(AudioDevice& engine, SynthVoice& voice, const std::string& waveName) {
//     std::atomic<bool> moveToNext{false};

//     std::thread inputThread([&moveToNext]() {
//         std::cin.get();
//         moveToNext = true;
//     });

//     std::cout << "\n========================================" << std::endl;
//     std::cout << " INITIALIZING OSCILLATOR: " << waveName << std::endl;
//     std::cout << " Press [ENTER] to advance to the next waveform." << std::endl;
//     std::cout << "========================================" << std::endl;

//     engine.start();

//     int currentMode = 0;

//     while (!moveToNext) {
//         // Dynamically reconfigure the voice parameters based on the current sequence mode
//         switch (currentMode) {
//             case 0:
//                 std::cout << "\n [Mode 1/4] Raw Oscillating Wave (Gate Envelope, No Filter)" << std::endl;
//                 voice.setDelay(0.0f);
//                 // Instant attack and release, full sustain to simulate a basic gate
//                 voice.getAmpEnvelope().setParameters(0.01f, 1.0f, 0.0f, 1.0f, 0.01f);
//                 // Bypass filter by opening it entirely
//                 voice.setFilterParameters(20000.0f, 0.0f); 
//                 voice.setFilterResonance(0.0f);
//                 break;

//             case 1:
//                 std::cout << "\n [Mode 2/4] Amplitude Envelope Applied (AHDSR)" << std::endl;
//                 voice.setDelay(0.5f);
//                 voice.getAmpEnvelope().setParameters(1.0f, 0.5f, 1.0f, 0.2f, 2.0f);
//                 voice.setFilterParameters(20000.0f, 0.0f);
//                 voice.setFilterResonance(0.0f);
//                 break;

//             case 2:
//                 std::cout << "\n [Mode 3/4] 4-Pole Low Pass Filter Sweep Added (No Resonance)" << std::endl;
//                 voice.setDelay(0.5f);
//                 voice.getAmpEnvelope().setParameters(1.0f, 0.5f, 1.0f, 0.2f, 2.0f);
//                 voice.setFilterParameters(100.0f, 6000.0f); // Sweep from 100Hz to 6100Hz
//                 voice.setFilterResonance(0.0f);
//                 break;

//             case 3:
//                 std::cout << "\n [Mode 4/4] Resonance Added (Analog-style Squelch)" << std::endl;
//                 voice.setDelay(0.5f);
//                 voice.getAmpEnvelope().setParameters(1.0f, 0.5f, 1.0f, 0.2f, 2.0f);
//                 voice.setFilterParameters(100.0f, 6000.0f);
//                 // Set resonance high enough to generate a sharp peak, keeping below self-oscillation (4.0)
//                 voice.setFilterResonance(3.5f); 
//                 break;
//         }

//         std::cout << "  -> Note ON" << std::endl;
//         voice.triggerNote();

//         // Hold phase: Wait 3.0 seconds for Delay -> Attack -> Hold -> Decay
//         for (int i = 0; i < 30; ++i) {
//             if (moveToNext) break;
//             SDL_Delay(100); 
//         }

//         if (moveToNext) break;

//         std::cout << "  -> Note OFF" << std::endl;
//         voice.releaseNote();

//         // Release phase: Wait 2.5 seconds for amplitude to return to 0.0
//         for (int i = 0; i < 25; ++i) {
//             if (moveToNext) break;
//             SDL_Delay(100);
//         }

//         // Advance and wrap the mode counter
//         currentMode = (currentMode + 1) % 4;
//     }

//     engine.stop();
//     inputThread.join(); 
// }

// int main(int argc, char* argv[]) {
//     SDL_SetMainReady();
    
//     if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
//         std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
//         return 1;
//     }

//     const float sampleRate = 44100.0f;
//     const float frequency = 110.0f; 

//     // Initialize core oscillators
//     SineOscillator sineWave(sampleRate, frequency);
//     SquareOscillator squareWave(sampleRate, frequency);
//     SawOscillator sawWave(sampleRate, frequency);
//     TriangleOscillator triWave(sampleRate, frequency);
//     NoiseGenerator noiseWave(0.1f);

//     // Ensure the filter envelope uses standard settings for the sequence
//     const float fAtt = 1.0f;
//     const float fHold = 0.0f;
//     const float fDec = 1.5f;
//     const float fSus = 0.1f;
//     const float fRel = 2.0f;

//     // 1. Sine
//     {
//         SynthVoice voice(&sineWave);
//         voice.setSampleRate(sampleRate);
//         voice.getFilterEnvelope().setParameters(fAtt, fHold, fDec, fSus, fRel);
        
//         AudioDevice audioEngine;
//         if (audioEngine.initialize(&voice)) runOscillatorTestSequence(audioEngine, voice, "SINE WAVE");
//     }

//     // 2. Square
//     {
//         SynthVoice voice(&squareWave);
//         voice.setSampleRate(sampleRate);
//         voice.getFilterEnvelope().setParameters(fAtt, fHold, fDec, fSus, fRel);
        
//         AudioDevice audioEngine;
//         if (audioEngine.initialize(&voice)) runOscillatorTestSequence(audioEngine, voice, "SQUARE WAVE");
//     }

//     // 3. Sawtooth
//     {
//         SynthVoice voice(&sawWave);
//         voice.setSampleRate(sampleRate);
//         voice.getFilterEnvelope().setParameters(fAtt, fHold, fDec, fSus, fRel);
        
//         AudioDevice audioEngine;
//         if (audioEngine.initialize(&voice)) runOscillatorTestSequence(audioEngine, voice, "SAWTOOTH WAVE");
//     }

//     // 4. Triangle
//     {
//         SynthVoice voice(&triWave);
//         voice.setSampleRate(sampleRate);
//         voice.getFilterEnvelope().setParameters(fAtt, fHold, fDec, fSus, fRel);
        
//         AudioDevice audioEngine;
//         if (audioEngine.initialize(&voice)) runOscillatorTestSequence(audioEngine, voice, "TRIANGLE WAVE");
//     }

//     // 5. Noise
//     {
//         SynthVoice voice(&noiseWave);
//         voice.setSampleRate(sampleRate);
//         voice.getFilterEnvelope().setParameters(fAtt, fHold, fDec, fSus, fRel);
        
//         AudioDevice audioEngine;
//         if (audioEngine.initialize(&voice)) runOscillatorTestSequence(audioEngine, voice, "NOISE");
//     }

//     std::cout << "\nSequence complete. Terminating DAW audio engine..." << std::endl;
//     SDL_Quit();
//     return 0;
// }