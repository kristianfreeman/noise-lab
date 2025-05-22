#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * ADSR Envelope generator for the noise generator.
 */
class EnvelopeGenerator
{
public:
    //==============================================================================
    EnvelopeGenerator();
    ~EnvelopeGenerator();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    void reset();

    //==============================================================================
    void noteOn(int midiNoteNumber, float velocity);
    void noteOff(int midiNoteNumber);
    
    //==============================================================================
    void setParameters(float attackTimeMs, float decayTimeMs, float sustainLevel, float releaseTimeMs);
    
    float getAttackTime() const;
    float getDecayTime() const;
    float getSustainLevel() const;
    float getReleaseTime() const;
    
    //==============================================================================
    void setOneShot(bool isOneShot);
    bool isOneShot() const;
    
    //==============================================================================
    bool isActive() const;
    
private:
    //==============================================================================
    enum EnvelopeStage
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };
    
    //==============================================================================
    double sampleRate;
    
    float attackTime;  // ms
    float decayTime;   // ms
    float sustainLevel; // 0 to 1
    float releaseTime; // ms
    
    EnvelopeStage currentStage;
    float currentLevel;
    float currentVelocity;
    
    float attackRate;  // change per sample
    float decayRate;   // change per sample
    float releaseRate; // change per sample
    
    bool oneShot;
    bool noteIsOn;
    
    //==============================================================================
    void calculateRates();
};