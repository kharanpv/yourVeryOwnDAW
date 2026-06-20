#include "InputStateManager.h"

InputStateManager::InputStateManager()
    : isLatched(false),
      activeKnobDirection(GrooveboxAction::NONE),
      knobStartTime(0),
      lastUpdateTicks(0),
      holdThresholdMs(250),
      baseSpeed(0.005f),
      accelerationCurve(4.0f) {}

void InputStateManager::processEvent(GrooveboxAction action, bool isDown) {
    if (action == GrooveboxAction::NONE) return;

    // TODO: Handle Latch Toggle logic here

    if (isDown) {
        activeKnobDirection = action;
        knobStartTime = SDL_GetTicks();
        lastUpdateTicks = knobStartTime;
    } else {
        // Only stop if releasing the actively tracked key
        if (activeKnobDirection == action) {
            activeKnobDirection = GrooveboxAction::NONE;
        }
    }
}

float InputStateManager::updateContinuous(Uint32 currentTicks) {
    if (activeKnobDirection == GrooveboxAction::NONE) return 0.0f;

    Uint32 holdDuration = currentTicks - knobStartTime;
    
    // Ignore short taps below the threshold
    if (holdDuration < holdThresholdMs) {
        lastUpdateTicks = currentTicks;
        return 0.0f; 
    }

    Uint32 dt = currentTicks - lastUpdateTicks;
    lastUpdateTicks = currentTicks;

    float timeSeconds = (holdDuration - holdThresholdMs) / 1000.0f;
    
    // Apply quadratic acceleration
    float multiplier = 1.0f + (timeSeconds * timeSeconds * accelerationCurve);
    float delta = baseSpeed * multiplier * dt;

    // Invert delta for downward actions
    if (activeKnobDirection == GrooveboxAction::CUTOFF_DOWN ||
        activeKnobDirection == GrooveboxAction::RES_DOWN ||
        activeKnobDirection == GrooveboxAction::ATTACK_DOWN) {
        delta = -delta;
    }

    return delta;
}

void InputStateManager::setHoldThreshold(Uint32 thresholdMs) { holdThresholdMs = thresholdMs; }
void InputStateManager::setBaseSpeed(float speed) { baseSpeed = speed; }
void InputStateManager::setAccelerationCurve(float curveSteepness) { accelerationCurve = curveSteepness; }
bool InputStateManager::getIsLatched() const { return isLatched; }