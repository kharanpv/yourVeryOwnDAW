#include "AhdsrEnvelope.h"

// Constructor initializes the envelope in a silent, Idle state.
AhdsrEnvelope::AhdsrEnvelope() 
    : state(State::Idle), sampleRate(44100.0f), currentLevel(0.0f),
      attackInc(0.0f), decayInc(0.0f), releaseInc(0.0f), 
      holdSampleCount(0.0f), currentHoldSample(0.0f), sustainLevel(1.0f) {}

// Updates the sample rate and recalculates the increments to keep timing accurate.
void AhdsrEnvelope::setSampleRate(float newSampleRate) {
    sampleRate = newSampleRate;
}

// Converts real-world time (seconds) into digital math (increments & sample counts).
void AhdsrEnvelope::setParameters(float attackSec, float holdSec, float decaySec, float sustainLvl, float releaseSec) {
    sustainLevel = sustainLvl;

    // Increments for ramps
    attackInc = (attackSec > 0.0f) ? (1.0f / (attackSec * sampleRate)) : 1.0f;
    decayInc = (decaySec > 0.0f) ? ((1.0f - sustainLevel) / (decaySec * sampleRate)) : 1.0f;
    releaseInc = (releaseSec > 0.0f) ? (sustainLevel / (releaseSec * sampleRate)) : 1.0f;
    
    // Total samples to wait during the Hold phase
    holdSampleCount = holdSec * sampleRate;
}

// Simulates a user pressing a physical key down.
void AhdsrEnvelope::triggerOn() {
    state = State::Attack;
    currentHoldSample = 0.0f; // Reset hold timer
}

// Simulates a user releasing a physical key.
void AhdsrEnvelope::triggerOff() {
    if (state != State::Idle) {
        state = State::Release;
    }
}

// The core state machine logic. Called once per audio frame.
float AhdsrEnvelope::process() {
    switch (state) {
        case State::Idle:
            currentLevel = 0.0f;
            break;

        case State::Attack:
            currentLevel += attackInc;
            if (currentLevel >= 1.0f) {
                currentLevel = 1.0f;
                // If there is a Hold time, go to Hold. Otherwise, skip straight to Decay.
                if (holdSampleCount > 0.0f) {
                    state = State::Hold;
                    currentHoldSample = 0.0f;
                } else {
                    state = State::Decay;
                }
            }
            break;

        case State::Hold:
            currentLevel = 1.0f;
            currentHoldSample += 1.0f;
            if (currentHoldSample >= holdSampleCount) {
                state = State::Decay;
            }
            break;

        case State::Decay:
            currentLevel -= decayInc;
            if (currentLevel <= sustainLevel) {
                currentLevel = sustainLevel;
                state = State::Sustain;
            }
            break;

        case State::Sustain:
            currentLevel = sustainLevel;
            break;

        case State::Release:
            currentLevel -= releaseInc;
            if (currentLevel <= 0.0f) {
                currentLevel = 0.0f;
                state = State::Idle;
            }
            break;
    }
    
    return currentLevel;
}

AhdsrEnvelope::State AhdsrEnvelope::getCurrentState() const {
    return state;
}