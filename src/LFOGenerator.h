#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Low Frequency Oscillator for modulation.
 */
class LFOGenerator
{
public:
    //==============================================================================
    enum LFOTarget
    {
        Volume = 0,
        FilterCutoff,
        FilterResonance,
        Pitch,
        NumTargets
    };

    //==============================================================================
    LFOGenerator();
    ~LFOGenerator();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    float processSample();
    void reset();

    //==============================================================================
    void setRate(float rateHz);
    void setDepth(float depth);
    void setTarget(LFOTarget target);
    void setSyncToHost(bool shouldSync);
    
    float getRate() const;
    float getDepth() const;
    LFOTarget getTarget() const;
    bool getSyncToHost() const;
    
    //==============================================================================
    void setHostBPM(double bpm);
    void setHostPPQPosition(double ppqPosition);
    
private:
    //==============================================================================
    double sampleRate;
    
    float rate; // Hz
    float depth; // 0 to 1
    LFOTarget target;
    bool syncToHost;
    
    // Host timing info
    double hostBPM;
    double hostPPQPosition;
    
    // Internal state
    float phase;
    float phaseIncrement;
    
    //==============================================================================
    void updatePhaseIncrement();
};