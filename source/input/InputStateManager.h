#pragma once
#include <SDL2/SDL.h>
#include "GrooveboxAction.h"

class InputStateManager {
public:
    InputStateManager();

    void processEvent(GrooveboxAction action, bool isDown);
    float updateContinuous(Uint32 currentTicks);
    bool getIsLatched() const;

    // Consume-once edge events — call once per event loop iteration.
    // Each returns its value and resets the pending state.
    bool consumeLatchToggle();
    int  consumeNoteOn();   // returns note index 0-11, or -1 if none pending
    int  consumeNoteOff();  // returns note index 0-11, or -1 if none pending

    // Returns the index into main.cpp's PARAM_RANGES table for the currently
    // held knob action (0=cutoff, 1=res, 2=attack), or -1 if no knob is held.
    int  getActiveParamIndex() const;

    // Waveform selection — consume the pending waveform change.
    // Returns 0-4 for the 5 waveform types, or -1 if no change is pending.
    int  consumeWaveformChange();

    void setHoldThreshold(Uint32 thresholdMs);
    void setBaseSpeed(float speed);
    void setTapStep(float step);
    void setAccelerationCurve(float curveSteepness);

private:
    static bool isKnobAction(GrooveboxAction a);

    bool isLatched;
    GrooveboxAction activeKnobDirection;
    Uint32 knobStartTime;
    Uint32 lastUpdateTicks;
    Uint32 holdThresholdMs;
    float  baseSpeed;
    float  tapStep;
    float  accelerationCurve;

    // On key-down, we emit a one-shot delta so the user gets immediate feedback.
    // This is consumed by the main loop so it doesn't repeat.
    bool  pendingOneShotKnobChange;

    bool pendingLatchToggle;
    int  pendingNoteOn;   // -1 if none pending
    int  pendingNoteOff;  // -1 if none pending
    int  pendingWaveformChange; // -1 if none pending
};
