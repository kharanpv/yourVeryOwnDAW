#include "InputStateManager.h"

InputStateManager::InputStateManager()
    : isLatched(false),
      activeKnobDirection(GrooveboxAction::NONE),
      knobStartTime(0),
      lastUpdateTicks(0),
      // --- Default Tuning Values ---
      holdThresholdMs(250),
      baseSpeed(0.005f),
      accelerationCurve(4.0f) {} // 4.0 is a good baseline, but now you can change it!

// ... [processEvent remains exactly the same] ...

float InputStateManager::updateContinuous(Uint32 currentTicks) {
    if (activeKnobDirection == GrooveboxAction::NONE) return 0.0f;

    Uint32 holdDuration = currentTicks - knobStartTime;
    
    // Use the variable instead of a hardcoded 250
    if (holdDuration < holdThresholdMs) {
        lastUpdateTicks = currentTicks;
        return 0.0f; 
    }

    Uint32 dt = currentTicks - lastUpdateTicks;
    lastUpdateTicks = currentTicks;

    float timeSeconds = (holdDuration - holdThresholdMs) / 1000.0f;
    
    // --- THE DYNAMIC ACCELERATION MATH ---
    // We now multiply by your adjustable curve variable!
    // A higher number means it flies to maximum speed faster.
    float multiplier = 1.0f + (timeSeconds * timeSeconds * accelerationCurve);

    // Use the adjustable base speed
    float delta = baseSpeed * multiplier * dt;

    // Negate the delta if the active action is a "DOWN" movement
    if (activeKnobDirection == GrooveboxAction::CUTOFF_DOWN ||
        activeKnobDirection == GrooveboxAction::RES_DOWN ||
        activeKnobDirection == GrooveboxAction::ATTACK_DOWN) {
        delta = -delta;
    }

    return delta;
}

// --- Setters ---
void InputStateManager::setHoldThreshold(Uint32 thresholdMs) { holdThresholdMs = thresholdMs; }
void InputStateManager::setBaseSpeed(float speed) { baseSpeed = speed; }
void InputStateManager::setAccelerationCurve(float curveSteepness) { accelerationCurve = curveSteepness; }
bool InputStateManager::getIsLatched() const { return isLatched; }