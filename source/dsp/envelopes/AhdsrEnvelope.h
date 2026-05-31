#pragma once

// AhdsrEnvelope
// A mathematical state-machine that generates a control signal between 0.0 and 1.0.
// Used to shape the amplitude (or other parameters) of a sound over time.
class AhdsrEnvelope {
public:
    enum class State { Idle, Attack, Hold, Decay, Sustain, Release };

    AhdsrEnvelope();

    // Must be called so the envelope knows how fast time is moving.
    void setSampleRate(float newSampleRate);
    
    // Configures the five stages of the envelope. Times are in seconds, levels are 0.0 to 1.0.
    void setParameters(float attackSec, float holdSec, float decaySec, float sustainLvl, float releaseSec);

    // Transitions the state machine.
    void triggerOn();
    void triggerOff();

    // Calculates and returns the current envelope multiplier for a single sample.
    float process();

    State getCurrentState() const;

private:
    State state;
    float sampleRate;
    float currentLevel;

    // The amount the level changes per single audio sample.
    float attackInc;
    float decayInc;
    float releaseInc;
    
    // Timer variables for the Hold phase
    float holdSampleCount;
    float currentHoldSample;
    
    float sustainLevel;
};