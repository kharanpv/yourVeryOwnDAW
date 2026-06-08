#pragma once

#include <SDL.h>
#include <string>

class AppWindow {
public:
    AppWindow();
    ~AppWindow();

    // Boots up the SDL subsystems, creates the window, and attaches a hardware renderer.
    bool initialize(const std::string& title, int width, int height);

    // Polls the operating system for events (like clicking the 'X' button or resizing).
    void processEvents();

    // Wipes the previous frame clean with a dark grey color.
    void clear();

    // Pushes the newly drawn pixels to the physical monitor.
    void present();

    // Safely tears down the SDL window and renderer.
    void shutdown();

    // Returns true as long as the user hasn't tried to close the window.
    bool isRunning() const { return running; }

    // Getters needed later for hooking up Dear ImGui
    SDL_Window* getSDLWindow() const { return window; }
    SDL_Renderer* getSDLRenderer() const { return renderer; }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
};