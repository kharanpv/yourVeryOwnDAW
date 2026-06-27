#include "InputStateManager.h"

InputStateManager::InputStateManager()
    : isLatched(false),
      activeKnobDirection(GrooveboxAction::NONE),
      knobStartTime(0),
      lastUpdateTicks(0),
      holdThresholdMs(250),
      baseSpeed(0.005f),
      tapStep(0.2f),
      accelerationCurve(4.0f),
      pendingOneShotKnobChange(false),
      pendingLatchToggle(false),
      pendingNoteOn(-1),
      pendingNoteOff(-1) {}

// --- Private helper ---

bool InputStateManager::isKnobAction(GrooveboxAction a) {
    return a == GrooveboxAction::CUTOFF_UP   ||
           a == GrooveboxAction::CUTOFF_DOWN ||
           a == GrooveboxAction::RES_UP      ||
           a == GrooveboxAction::RES_DOWN    ||
           a == GrooveboxAction::ATTACK_UP   ||
           a == GrooveboxAction::ATTACK_DOWN;
}

// --- Event processing ---

void InputStateManager::processEvent(GrooveboxAction action, bool isDown) {
    if (action == GrooveboxAction::NONE) return;

    if (action == GrooveboxAction::TOGGLE_LATCH) {
        if (isDown) {
            isLatched = !isLatched;
            pendingLatchToggle = true;
        }
        return;
    }

    if (action >= GrooveboxAction::AUDITION_NOTE_0 &&
        action <= GrooveboxAction::AUDITION_NOTE_11) {
        int noteIndex = static_cast<int>(action) -
                        static_cast<int>(GrooveboxAction::AUDITION_NOTE_0);
        if (isDown) {
            pendingNoteOn = noteIndex;
        } else {
            pendingNoteOff = noteIndex;
        }
        return;
    }

    if (isKnobAction(action)) {
        if (isDown) {
            activeKnobDirection = action;
            knobStartTime = SDL_GetTicks();
            lastUpdateTicks = knobStartTime;
            // Emit one immediate step so the user feels immediate feedback
            pendingOneShotKnobChange = true;
        } else {
            if (activeKnobDirection == action) {
                activeKnobDirection = GrooveboxAction::NONE;
            }
        }
    }
}

// --- Continuous knob update ---

float InputStateManager::updateContinuous(Uint32 currentTicks) {
    if (!isKnobAction(activeKnobDirection)) return 0.0f;

    // --- One-shot on key-down: immediate single step (tap behavior) ---
    if (pendingOneShotKnobChange) {
        pendingOneShotKnobChange = false;
        float sign = (activeKnobDirection == GrooveboxAction::CUTOFF_DOWN ||
                      activeKnobDirection == GrooveboxAction::RES_DOWN    ||
                      activeKnobDirection == GrooveboxAction::ATTACK_DOWN) ? -1.0f : 1.0f;
        return sign * tapStep;
    }

    // --- Hold behavior: 250 ms dead-zone, then accelerating stream ---
    Uint32 holdDuration = currentTicks - knobStartTime;
    if (holdDuration < holdThresholdMs) {
        lastUpdateTicks = currentTicks;
        return 0.0f;
    }

    Uint32 dt = currentTicks - lastUpdateTicks;
    lastUpdateTicks = currentTicks;

    // Linear ramp: starts at half tap-step per frame and ramps up smoothly.
    // The longer you hold, faster it goes — no quadratic explosion.
    float holdSeconds = (holdDuration - holdThresholdMs) / 1000.0f;
    float speed = baseSpeed * (1.0f + holdSeconds * accelerationCurve);
    float delta = speed * dt;

    if (activeKnobDirection == GrooveboxAction::CUTOFF_DOWN ||
        activeKnobDirection == GrooveboxAction::RES_DOWN    ||
        activeKnobDirection == GrooveboxAction::ATTACK_DOWN) {
        delta = -delta;
    }

    return delta;
}

// --- Consume-once edge events ---

bool InputStateManager::consumeLatchToggle() {
    if (pendingLatchToggle) {
        pendingLatchToggle = false;
        return true;
    }
    return false;
}

int InputStateManager::consumeNoteOn() {
    int note = pendingNoteOn;
    pendingNoteOn = -1;
    return note;
}

int InputStateManager::consumeNoteOff() {
    int note = pendingNoteOff;
    pendingNoteOff = -1;
    return note;
}

// --- Getters / setters ---

int InputStateManager::getActiveParamIndex() const {
    switch (activeKnobDirection) {
        case GrooveboxAction::CUTOFF_UP:   return 0;
        case GrooveboxAction::CUTOFF_DOWN: return 0;
        case GrooveboxAction::RES_UP:      return 1;
        case GrooveboxAction::RES_DOWN:    return 1;
        case GrooveboxAction::ATTACK_UP:   return 2;
        case GrooveboxAction::ATTACK_DOWN: return 2;
        default:                           return -1;
    }
}

bool InputStateManager::getIsLatched() const { return isLatched; }

void InputStateManager::setHoldThreshold(Uint32 thresholdMs) { holdThresholdMs = thresholdMs; }
void InputStateManager::setTapStep(float step)                 { tapStep = step; }
void InputStateManager::setBaseSpeed(float speed)             { baseSpeed = speed; }
void InputStateManager::setAccelerationCurve(float curveSteepness) { accelerationCurve = curveSteepness; }
