#include "LFOGenerator.h"

//==============================================================================
LFOGenerator::LFOGenerator()
    : sampleRate(44100.0)
    , rate(1.0f)        // Default: 1 Hz
    , depth(0.5f)       // Default: 50%
    , target(Volume)    // Default: Volume
    , syncToHost(false) // Default: not synced
    , hostBPM(120.0)    // Default: 120 BPM
    , hostPPQPosition(0.0)
    , phase(0.0f)
    , phaseIncrement(0.0f)
{
    updatePhaseIncrement();
}

LFOGenerator::~LFOGenerator()
{
}

//==============================================================================
void LFOGenerator::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    updatePhaseIncrement();
    reset();
}

float LFOGenerator::processSample()
{
    // Move the phase forward
    phase += phaseIncrement;
    if (phase >= 1.0f)
        phase -= 1.0f;
    
    // Generate a sine wave
    return depth * std::sin(2.0f * juce::MathConstants<float>::pi * phase);
}

void LFOGenerator::reset()
{
    phase = 0.0f;
}

//==============================================================================
void LFOGenerator::setRate(float rateHz)
{
    rate = rateHz;
    updatePhaseIncrement();
}

void LFOGenerator::setDepth(float newDepth)
{
    depth = juce::jlimit(0.0f, 1.0f, newDepth);
}

void LFOGenerator::setTarget(LFOTarget newTarget)
{
    target = newTarget;
}

void LFOGenerator::setSyncToHost(bool shouldSync)
{
    syncToHost = shouldSync;
    updatePhaseIncrement();
}

float LFOGenerator::getRate() const
{
    return rate;
}

float LFOGenerator::getDepth() const
{
    return depth;
}

LFOGenerator::LFOTarget LFOGenerator::getTarget() const
{
    return target;
}

bool LFOGenerator::getSyncToHost() const
{
    return syncToHost;
}

//==============================================================================
void LFOGenerator::setHostBPM(double bpm)
{
    hostBPM = bpm;
    if (syncToHost)
        updatePhaseIncrement();
}

void LFOGenerator::setHostPPQPosition(double ppqPosition)
{
    hostPPQPosition = ppqPosition;
    
    // If synced to host, we can use PPQ position to directly set the phase
    if (syncToHost)
    {
        // Normalize the PPQ position to a 0-1 phase
        phase = std::fmod(static_cast<float>(hostPPQPosition), 1.0f);
    }
}

//==============================================================================
void LFOGenerator::updatePhaseIncrement()
{
    if (syncToHost)
    {
        // When synced to host, we derive the rate from the BPM
        // For example, 1/4 note at 120 BPM = 2 Hz (120 BPM / 60 seconds)
        float syncedRate = static_cast<float>(hostBPM / 60.0);
        
        // Apply a multiplier based on the rate value
        // rate of 1.0 = quarter notes, 2.0 = eighth notes, 0.5 = half notes, etc.
        syncedRate *= rate;
        
        phaseIncrement = syncedRate / static_cast<float>(sampleRate);
    }
    else
    {
        // Normal operation: increment phase based on the rate in Hz
        phaseIncrement = rate / static_cast<float>(sampleRate);
    }
}