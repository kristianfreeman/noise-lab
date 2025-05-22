#include "EnvelopeGenerator.h"

//==============================================================================
EnvelopeGenerator::EnvelopeGenerator()
    : sampleRate(44100.0)
    , attackTime(10.0f)      // Default: 10ms
    , decayTime(100.0f)      // Default: 100ms
    , sustainLevel(0.7f)     // Default: 0.7
    , releaseTime(500.0f)    // Default: 500ms
    , currentStage(Idle)
    , currentLevel(0.0f)
    , currentVelocity(0.0f)
    , attackRate(0.0f)
    , decayRate(0.0f)
    , releaseRate(0.0f)
    , oneShot(false)
    , noteIsOn(false)
{
    calculateRates();
}

EnvelopeGenerator::~EnvelopeGenerator()
{
}

//==============================================================================
void EnvelopeGenerator::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    calculateRates();
    reset();
}

void EnvelopeGenerator::processBlock(juce::AudioBuffer<float>& buffer, int numSamples)
{
    // If envelope is not active, zero the buffer and return
    if (currentStage == Idle && !noteIsOn)
    {
        buffer.clear();
        return;
    }
    
    // Process the envelope for each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Update envelope level based on current stage
        switch (currentStage)
        {
            case Attack:
                currentLevel += attackRate;
                if (currentLevel >= 1.0f)
                {
                    currentLevel = 1.0f;
                    currentStage = Decay;
                }
                break;
                
            case Decay:
                currentLevel -= decayRate;
                if (currentLevel <= sustainLevel)
                {
                    currentLevel = sustainLevel;
                    currentStage = Sustain;
                    
                    // For one-shot mode, immediately transition to release
                    if (oneShot)
                        currentStage = Release;
                }
                break;
                
            case Sustain:
                // Level stays constant at sustainLevel
                // If note is released, transition to release stage
                if (!noteIsOn)
                    currentStage = Release;
                break;
                
            case Release:
                currentLevel -= releaseRate;
                if (currentLevel <= 0.0f)
                {
                    currentLevel = 0.0f;
                    currentStage = Idle;
                }
                break;
                
            case Idle:
                // Do nothing
                break;
        }
        
        // Apply envelope to all channels
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            channelData[sample] *= currentLevel * currentVelocity;
        }
    }
}

void EnvelopeGenerator::reset()
{
    currentStage = Idle;
    currentLevel = 0.0f;
    noteIsOn = false;
}

//==============================================================================
void EnvelopeGenerator::noteOn(int /*midiNoteNumber*/, float velocity)
{
    currentVelocity = velocity;
    noteIsOn = true;
    
    // Start the attack phase
    currentStage = Attack;
}

void EnvelopeGenerator::noteOff(int /*midiNoteNumber*/)
{
    noteIsOn = false;
    
    // If we're in a sustain stage, move to release
    if (currentStage == Sustain)
    {
        currentStage = Release;
    }
}

//==============================================================================
void EnvelopeGenerator::setParameters(float newAttackTimeMs, float newDecayTimeMs, 
                                       float newSustainLevel, float newReleaseTimeMs)
{
    attackTime = newAttackTimeMs;
    decayTime = newDecayTimeMs;
    sustainLevel = juce::jlimit(0.0f, 1.0f, newSustainLevel);
    releaseTime = newReleaseTimeMs;
    
    calculateRates();
}

float EnvelopeGenerator::getAttackTime() const
{
    return attackTime;
}

float EnvelopeGenerator::getDecayTime() const
{
    return decayTime;
}

float EnvelopeGenerator::getSustainLevel() const
{
    return sustainLevel;
}

float EnvelopeGenerator::getReleaseTime() const
{
    return releaseTime;
}

//==============================================================================
void EnvelopeGenerator::setOneShot(bool isOneShot)
{
    oneShot = isOneShot;
}

bool EnvelopeGenerator::isOneShot() const
{
    return oneShot;
}

//==============================================================================
bool EnvelopeGenerator::isActive() const
{
    return currentStage != Idle;
}

//==============================================================================
void EnvelopeGenerator::calculateRates()
{
    // Convert times from milliseconds to per-sample rates
    // For attack, go from 0 to 1 in attackTime
    attackRate = 1.0f / (attackTime * 0.001f * static_cast<float>(sampleRate));
    
    // For decay, go from 1 to sustainLevel in decayTime
    decayRate = (1.0f - sustainLevel) / (decayTime * 0.001f * static_cast<float>(sampleRate));
    
    // For release, go from current level to 0 in releaseTime
    releaseRate = 1.0f / (releaseTime * 0.001f * static_cast<float>(sampleRate));
}