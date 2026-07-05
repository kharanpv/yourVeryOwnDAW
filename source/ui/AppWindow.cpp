#include "AppWindow.h"
#include <iostream>

// --- ImGui Headers ---
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#ifdef _WIN32
#include <windows.h>
#endif

AppWindow::AppWindow() : window(nullptr), renderer(nullptr), running(false) {}

AppWindow::~AppWindow() {
    shutdown();
}

bool AppWindow::initialize(const std::string& title, int width, int height) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL Video: " << SDL_GetError() << std::endl;
        return false;
    }

#ifdef _WIN32
    // Tell Windows we handle DPI scaling ourselves. Without this, Windows
    // DPI-virtualizes the app (treating the 2560x1600 display as 1280x800)
    // and scales the window up by 2x, doubling our requested size.
    SetProcessDPIAware();
#endif

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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // On high-DPI displays (where we called SetProcessDPIAware), the logical
    // pixel grid is 1:1 with physical pixels, so ImGui renders everything at
    // 1x by default. Scale fonts and style up 2x so the UI is readable and
    // proportionally matches the pre-DPI-awareness appearance.
    io.FontGlobalScale = 2.0f;

    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(2.0f);

    // Connect ImGui to our SDL Window and Renderer
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    running = true;
    return true;
}

// Pass the raw SDL event pointer directly into the ImGui backend.
// We strictly separate OS window management from UI event handling here.
void AppWindow::processImGuiEvent(const SDL_Event* event) {
    ImGui_ImplSDL2_ProcessEvent(event);
}

// Prepare ImGui for a new frame of logic
void AppWindow::beginUiFrame() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void AppWindow::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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
