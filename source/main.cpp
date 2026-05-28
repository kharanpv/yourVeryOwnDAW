// Tell SDL not to hijack the main entry point
#define SDL_MAIN_HANDLED

#include "core/AudioDevice.h"
#include "dsp/oscillators/SineOscillator.h"
#include "dsp/oscillators/SquareOscillator.h"
#include "dsp/oscillators/SawOscillator.h"
#include "dsp/oscillators/TriangleOscillator.h"
#include "dsp/oscillators/NoiseGenerator.h"
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    SDL_SetMainReady();
    
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Common parameters
    float sampleRate = 44100.0f;
    float frequency = 440.0f; // Middle A

    // Instantiate all waveforms
    SineOscillator sineWave(sampleRate, frequency);
    SquareOscillator squareWave(sampleRate, frequency);
    SawOscillator sawWave(sampleRate, frequency);
    TriangleOscillator triWave(sampleRate, frequency);
    NoiseGenerator noiseWave(0.1f); // Noise has no frequency, only amplitude

    // 1. Sine
    {
        AudioDevice audioEngine;
        if (audioEngine.initialize(&sineWave)) {
            std::cout << "Playing SINE wave. Press Enter to switch to SQUARE..." << std::endl;
            audioEngine.start();
            std::cin.get();
            audioEngine.stop();
        }
    }

    // 2. Square
    {
        AudioDevice audioEngine;
        if (audioEngine.initialize(&squareWave)) {
            std::cout << "Playing SQUARE wave. Press Enter to switch to SAWTOOTH..." << std::endl;
            audioEngine.start();
            std::cin.get();
            audioEngine.stop();
        }
    }

    // 3. Sawtooth
    {
        AudioDevice audioEngine;
        if (audioEngine.initialize(&sawWave)) {
            std::cout << "Playing SAWTOOTH wave. Press Enter to switch to TRIANGLE..." << std::endl;
            audioEngine.start();
            std::cin.get();
            audioEngine.stop();
        }
    }

    // 4. Triangle
    {
        AudioDevice audioEngine;
        if (audioEngine.initialize(&triWave)) {
            std::cout << "Playing TRIANGLE wave. Press Enter to switch to NOISE..." << std::endl;
            audioEngine.start();
            std::cin.get();
            audioEngine.stop();
        }
    }

    // 5. Noise
    {
        AudioDevice audioEngine;
        if (audioEngine.initialize(&noiseWave)) {
            std::cout << "Playing NOISE. Press Enter to TERMINATE." << std::endl;
            audioEngine.start();
            std::cin.get();
            audioEngine.stop();
        }
    }

    SDL_Quit();
    return 0;
}