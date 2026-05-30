// Tell SDL not to hijack the main entry point
#define SDL_MAIN_HANDLED

#include "core/AudioDevice.h"
#include "dsp/oscillators/SineOscillator.h"
#include "dsp/oscillators/SquareOscillator.h"
#include "dsp/oscillators/SawOscillator.h"
#include "dsp/oscillators/TriangleOscillator.h"
#include "dsp/oscillators/NoiseGenerator.h"

// IMPORTANT: Update this include path based on your folder restructuring
#include "dsp/SynthVoice.h" 

#include <SDL2/SDL.h>
#include <iostream>
#include <thread>
#include <atomic>

// Helper function to run an automated ADSR loop while waiting for user input
void runAdsrLoop(AudioDevice& engine, SynthVoice& voice, const std::string& waveName) {
    // Thread-safe boolean to signal when the user has pressed Enter
    std::atomic<bool> moveToNext{false};

    // Spawn a background thread to listen for the Enter key
    std::thread inputThread([&moveToNext]() {
        std::cin.get();
        moveToNext = true;
    });

    std::cout << "\n=== Playing " << waveName << " ===" << std::endl;
    std::cout << "Auto-looping ADSR. Press Enter ONCE to switch to the next wave..." << std::endl;

    engine.start();

    // The main thread acts as an automated sequencer
    while (!moveToNext) {
        std::cout << "  -> Triggering Note (Attack -> Decay -> Sustain)..." << std::endl;
        voice.triggerNote();

        // Wait 2.0 seconds (Allows Attack and Decay to finish)
        // We loop SDL_Delay in tiny chunks so the program can quit instantly if Enter is pressed
        for (int i = 0; i < 20; ++i) {
            if (moveToNext) break;
            SDL_Delay(100); 
        }

        if (moveToNext) break;

        std::cout << "  -> Releasing Note (Release phase)..." << std::endl;
        voice.releaseNote();

        // Wait 2.5 seconds for the sound to fade out completely before re-triggering
        for (int i = 0; i < 25; ++i) {
            if (moveToNext) break;
            SDL_Delay(100);
        }
    }

    engine.stop();
    inputThread.join(); // Safely clean up the input thread before moving to the next waveform
}

int main(int argc, char* argv[]) {
    SDL_SetMainReady();
    
    // We add SDL_INIT_TIMER so we can use SDL_Delay safely
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    float sampleRate = 44100.0f;
    float frequency = 440.0f; 

    SineOscillator sineWave(sampleRate, frequency);
    SquareOscillator squareWave(sampleRate, frequency);
    SawOscillator sawWave(sampleRate, frequency);
    TriangleOscillator triWave(sampleRate, frequency);
    NoiseGenerator noiseWave(0.1f);

    // We use a 1s Attack, 1s Decay, 20% Sustain, 2s Release
    // This perfectly fits inside the 4.5 second delay cycle in our runAdsrLoop function
    float att = 1.0f, dec = 1.0f, sus = 0.2f, rel = 2.0f;

    // 1. Sine Test
    {
        SynthVoice voice(&sineWave);
        voice.getEnvelope().setParameters(att, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAdsrLoop(audioEngine, voice, "SINE WAVE");
    }

    // 2. Square Test
    {
        SynthVoice voice(&squareWave);
        voice.getEnvelope().setParameters(att, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAdsrLoop(audioEngine, voice, "SQUARE WAVE");
    }

    // 3. Sawtooth Test
    {
        SynthVoice voice(&sawWave);
        voice.getEnvelope().setParameters(att, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAdsrLoop(audioEngine, voice, "SAWTOOTH WAVE");
    }

    // 4. Triangle Test
    {
        SynthVoice voice(&triWave);
        voice.getEnvelope().setParameters(att, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAdsrLoop(audioEngine, voice, "TRIANGLE WAVE");
    }

    // 5. Noise Test
    {
        SynthVoice voice(&noiseWave);
        voice.getEnvelope().setParameters(att, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAdsrLoop(audioEngine, voice, "NOISE");
    }

    std::cout << "\nTest complete. Terminating DAW..." << std::endl;
    SDL_Quit();
    return 0;
}