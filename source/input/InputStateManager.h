#pragma once
#include <SDL2/SDL.h>
#include "GrooveboxAction.h"

class InputStateManager {
public:
    InputStateManager();

    void processEvent(GrooveboxAction action, bool isDown);
    float updateContinuous(Uint32 currentTicks);
    bool getIsLatched() const;

    // --- Configuration Setters ---
    // Call these from your UI or config loader to tweak the knob "feel"
    void setHoldThreshold(Uint32 thresholdMs);
    void setBaseSpeed(float speed);
    void setAccelerationCurve(float curveSteepness);

private:
    bool isLatched;

    GrooveboxAction activeKnobDirection;
    Uint32 knobStartTime;
    Uint32 lastUpdateTicks;

    // --- Tuning Variables (No longer const!) ---
    Uint32 holdThresholdMs; 
    float baseSpeed;      
    float accelerationCurve; // Defines how violently the speed ramps up
};