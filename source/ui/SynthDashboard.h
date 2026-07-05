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
class WaveformPanel;
class EnvelopePanel;

class SynthDashboard {
public:
    SynthDashboard(std::shared_ptr<SharedMatrix> matrix,
                   std::shared_ptr<KeymapRouter> router);
    ~SynthDashboard();
    void render();

private:
    void setupTerminalTheme();
    void drawLeftColumn(int currentWave);
    void drawRightColumn(int currentWave);

    std::shared_ptr<SharedMatrix> dspMatrix;
    std::shared_ptr<KeymapRouter> keyRouter;

    // Reusable component instances (owned via unique_ptr)
    std::unique_ptr<ParamBox>         paramBox;
    std::unique_ptr<ToggleBox>        toggleBox;
    std::unique_ptr<WaveformSelector> waveformSelector;

    // Encapsulated panels (each owns its generators + ScopeCanvas)
    std::unique_ptr<WaveformPanel>  waveformPanel;
    std::unique_ptr<EnvelopePanel>  envelopePanel;
};