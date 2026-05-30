#include "AdsrEnvelope.h"

// Constructor initializes the envelope in a silent, Idle state.
AdsrEnvelope::AdsrEnvelope() 
    : state(State::Idle), sampleRate(44100.0f), currentLevel(0.0f),
      attackInc(0.0f), decayInc(0.0f), releaseInc(0.0f), sustainLevel(1.0f) {}

// Updates the sample rate and recalculates the increments to keep timing accurate.
void AdsrEnvelope::setSampleRate(float newSampleRate) {
    sampleRate = newSampleRate;
}

// Converts real-world time (seconds) into digital math (increments per sample).
void AdsrEnvelope::setParameters(float attackSec, float decaySec, float sustainLvl, float releaseSec) {
    sustainLevel = sustainLvl;

    // Formula: Increment = 1.0 / (Seconds * SampleRate)
    // We prevent division by zero by checking if the time is greater than 0.
    attackInc = (attackSec > 0.0f) ? (1.0f / (attackSec * sampleRate)) : 1.0f;
    decayInc = (decaySec > 0.0f) ? ((1.0f - sustainLevel) / (decaySec * sampleRate)) : 1.0f;
    releaseInc = (releaseSec > 0.0f) ? (sustainLevel / (releaseSec * sampleRate)) : 1.0f;
}

// Simulates a user pressing a physical key down.
void AdsrEnvelope::triggerOn() {
    state = State::Attack;
}

// Simulates a user releasing a physical key.
void AdsrEnvelope::triggerOff() {
    if (state != State::Idle) {
        state = State::Release;
    }
}

// The core state machine logic. Called once per audio frame.
float AdsrEnvelope::process() {
    switch (state) {
        case State::Idle:
            currentLevel = 0.0f;
            break;

        case State::Attack:
            currentLevel += attackInc;
            if (currentLevel >= 1.0f) {
                currentLevel = 1.0f;
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
            // Level remains constant during sustain phase.
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

AdsrEnvelope::State AdsrEnvelope::getCurrentState() const {
    return state;
}