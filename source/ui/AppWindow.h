#pragma once

#include <SDL.h>
#include <string>

class AppWindow {
public:
    AppWindow();
    ~AppWindow();

    // Boots up the SDL subsystems, creates the window, and attaches a hardware renderer.
    // Also initializes the Dear ImGui context and theme.
    bool initialize(const std::string& title, int width, int height);

    // Polls the operating system for events (like clicking the 'X' button or resizing).
    // Now also forwards mouse/keyboard events to ImGui so buttons can be clicked.
    void processEvents();

    // --- Dear ImGui Lifecycle ---
    // Prepares the ImGui engine to accept new UI commands for this frame.
    void beginUiFrame();

    // Wipes the previous frame clean with a dark grey hardware-synth color.
    void clear();

    // --- Dear ImGui Lifecycle ---
    // Calculates the ImGui geometry and renders it over the cleared background.
    void drawUi();

    // Pushes the newly drawn backbuffer pixels to the physical monitor.
    void present();

    // Safely tears down the ImGui context, SDL renderer, and SDL window.
    void shutdown();

    // Returns true as long as the user hasn't tried to close the window.
    bool isRunning() const { return running; }

    // Getters needed for engine wiring
    SDL_Window* getSDLWindow() const { return window; }
    SDL_Renderer* getSDLRenderer() const { return renderer; }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
};