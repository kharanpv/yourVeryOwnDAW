// Tell SDL not to hijack the main entry point
#define SDL_MAIN_HANDLED

#include "core/AudioDevice.h"
#include "dsp/oscillators/SineOscillator.h"
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize core OS systems via SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Instantiate the DSP (44100 Hz sample rate, 440 Hz = Middle A)
    SineOscillator sineWave(44100.0f, 440.0f);

    // Instantiate and configure the engine
    AudioDevice audioEngine;
    if (!audioEngine.initialize(&sineWave)) {
        return 1;
    }

    // Fire the engine
    std::cout << "Engine running. Press Enter to stop playback." << std::endl;
    audioEngine.start();

    // Block the main thread from exiting immediately
    std::cin.get();

    //  Teardown
    audioEngine.stop();
    SDL_Quit();

    return 0;
}