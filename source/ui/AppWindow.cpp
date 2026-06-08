#include "AppWindow.h"
#include <iostream>

AppWindow::AppWindow() : window(nullptr), renderer(nullptr), running(false) {}

AppWindow::~AppWindow() {
    shutdown();
}

bool AppWindow::initialize(const std::string& title, int width, int height) {
    // Initialize SDL Video subsystem (Audio is initialized elsewhere in your engine)
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL Video: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create the physical window
    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!window) {
        std::cerr << "Failed to create SDL Window: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create the hardware-accelerated 2D renderer locked to the monitor's refresh rate
    renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cerr << "Failed to create SDL Renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    running = true;
    return true;
}

void AppWindow::processEvents() {
    SDL_Event event;
    // Drain the OS event queue so the window doesn't freeze
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false; // User clicked the 'X' button
        }
        // Future: Pass keyboard events to the Input Router here
    }
}

void AppWindow::clear() {
    // Set the drawing color to a dark, hardware-synth grey (R: 30, G: 30, B: 30)
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    // Fill the screen with that color
    SDL_RenderClear(renderer);
}

void AppWindow::present() {
    // Push the backbuffer to the physical screen
    SDL_RenderPresent(renderer);
}

void AppWindow::shutdown() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}