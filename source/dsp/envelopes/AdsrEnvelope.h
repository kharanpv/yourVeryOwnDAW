#pragma once

// AdsrEnvelope
// A mathematical state-machine that generates a control signal between 0.0 and 1.0.
// Used to shape the amplitude (or other parameters) of a sound over time.
class AdsrEnvelope {
public:
    enum class State { Idle, Attack, Decay, Sustain, Release };

    AdsrEnvelope();

    // Must be called so the envelope knows how fast time is moving.
    void setSampleRate(float newSampleRate);
    
    // Configures the four stages of the envelope. Times are in seconds, levels are 0.0 to 1.0.
    void setParameters(float attackSec, float decaySec, float sustainLvl, float releaseSec);

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
    
    float sustainLevel;
};