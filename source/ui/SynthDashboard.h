#pragma once

#include <memory>
#include <string>
#include <imgui.h>
#include "../core/SharedMatrix.h"
#include "../input/KeymapRouter.h"

class ScopeCanvas;
class ParamBox;
class ToggleBox;
class WaveformSelector;
class WaveformGenerator;
class EnvelopeGenerator;

class SynthDashboard {
public:
    SynthDashboard(std::shared_ptr<SharedMatrix> matrix,
                   std::shared_ptr<KeymapRouter> router);
    ~SynthDashboard();
    void render();

private:
    // SynthDashboard(const SynthDashboard&) = delete;
    // SynthDashboard& operator=(const SynthDashboard&) = delete;

    void setupTerminalTheme();
    void drawLeftColumn(int currentWave);
    void drawRightColumn(int currentWave);
    void drawWaveformPlane(int waveType);
    void drawEnvelopeGraph();

    std::shared_ptr<SharedMatrix> dspMatrix;
    std::shared_ptr<KeymapRouter> keyRouter;

    // Reusable component instances (owned via unique_ptr)
    std::unique_ptr<ParamBox>         paramBox;
    std::unique_ptr<ToggleBox>        toggleBox;
    std::unique_ptr<WaveformSelector> waveformSelector;
    std::unique_ptr<ScopeCanvas>      scopeCanvas;
    std::unique_ptr<WaveformGenerator> waveGen;
    std::unique_ptr<EnvelopeGenerator> envGen;
};