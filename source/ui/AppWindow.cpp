#include "AppWindow.h"
#include <iostream>

// --- ImGui Headers ---
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

AppWindow::AppWindow() : window(nullptr), renderer(nullptr), running(false) {}

AppWindow::~AppWindow() {
    shutdown();
}

bool AppWindow::initialize(const std::string& title, int width, int height) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL Video: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!window) return false;

    renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) return false;

    // --- BOOT SEQUENCE: Initialize Dear ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Allow keyboard navigation

    ImGui::StyleColorsDark(); // Hardware Synth aesthetic

    // Connect ImGui to our SDL Window and Renderer
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    running = true;
    return true;
}

void AppWindow::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // MUST pass the event to ImGui so it knows where the mouse is!
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT) {
            running = false; 
        }
    }
}

// Prepare ImGui for a new frame of logic
void AppWindow::beginUiFrame() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void AppWindow::clear() {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);
}

// Tell ImGui to calculate its geometry and draw it over our cleared background
void AppWindow::drawUi() {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void AppWindow::present() {
    SDL_RenderPresent(renderer);
}

void AppWindow::shutdown() {
    // --- Clean up ImGui memory ---
    if (renderer) {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}