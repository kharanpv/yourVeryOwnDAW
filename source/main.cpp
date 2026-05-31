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

// Helper function to run an automated AHDSR loop while waiting for user input
void runAhdsrLoop(AudioDevice& engine, SynthVoice& voice, const std::string& waveName) {
    // Thread-safe boolean to signal when the user has pressed Enter
    std::atomic<bool> moveToNext{false};

    // Spawn a background thread to listen for the Enter key
    std::thread inputThread([&moveToNext]() {
        std::cin.get();
        moveToNext = true;
    });

    std::cout << "\n=== Playing " << waveName << " ===" << std::endl;
    std::cout << "Auto-looping AHDSR. Press Enter ONCE to switch to the next wave..." << std::endl;

    engine.start();

    // The main thread acts as an automated sequencer
    while (!moveToNext) {
        std::cout << "  -> Triggering Note (Delay -> Attack -> Hold -> Decay -> Sustain)..." << std::endl;
        voice.triggerNote();

        // Wait 3.0 seconds (Allows 0.5s Delay, 1.0s Attack, 0.5s Hold, and 1.0s Decay to finish)
        // We loop SDL_Delay in tiny chunks so the program can quit instantly if Enter is pressed
        for (int i = 0; i < 30; ++i) {
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
    float frequency = 110.0f; // Dropped to 110Hz (Low A) so Saw and Square sound fatter

    SineOscillator sineWave(sampleRate, frequency);
    SquareOscillator squareWave(sampleRate, frequency);
    SawOscillator sawWave(sampleRate, frequency);
    TriangleOscillator triWave(sampleRate, frequency);
    NoiseGenerator noiseWave(0.1f);

    // Our new 5-stage AHDSR + Pre-Delay configuration
    float delay = 0.5f;
    float att = 1.0f;
    float hold = 0.5f;
    float dec = 1.0f;
    float sus = 0.2f;
    float rel = 2.0f;

    // 1. Sine Test
    {
        SynthVoice voice(&sineWave);
        voice.setSampleRate(sampleRate);
        voice.setDelay(delay);
        voice.getEnvelope().setParameters(att, hold, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAhdsrLoop(audioEngine, voice, "SINE WAVE");
    }

    // 2. Square Test
    {
        SynthVoice voice(&squareWave);
        voice.setSampleRate(sampleRate);
        voice.setDelay(delay);
        voice.getEnvelope().setParameters(att, hold, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAhdsrLoop(audioEngine, voice, "SQUARE WAVE");
    }

    // 3. Sawtooth Test
    {
        SynthVoice voice(&sawWave);
        voice.setSampleRate(sampleRate);
        voice.setDelay(delay);
        voice.getEnvelope().setParameters(att, hold, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAhdsrLoop(audioEngine, voice, "SAWTOOTH WAVE");
    }

    // 4. Triangle Test
    {
        SynthVoice voice(&triWave);
        voice.setSampleRate(sampleRate);
        voice.setDelay(delay);
        voice.getEnvelope().setParameters(att, hold, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAhdsrLoop(audioEngine, voice, "TRIANGLE WAVE");
    }

    // 5. Noise Test
    {
        SynthVoice voice(&noiseWave);
        voice.setSampleRate(sampleRate);
        voice.setDelay(delay);
        voice.getEnvelope().setParameters(att, hold, dec, sus, rel);
        AudioDevice audioEngine;
        if (audioEngine.initialize(&voice)) runAhdsrLoop(audioEngine, voice, "NOISE");
    }

    std::cout << "\nTest complete. Terminating DAW..." << std::endl;
    SDL_Quit();
    return 0;
}