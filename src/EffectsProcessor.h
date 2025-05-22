#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Effects processor for the noise generator.
 */
class EffectsProcessor
{
public:
    //==============================================================================
    EffectsProcessor();
    ~EffectsProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    void reset();

    //==============================================================================
    void setDrive(float drive);
    void setBitcrush(float bitDepth);
    void setStereoWidth(float width);
    
    float getDrive() const;
    float getBitcrush() const;
    float getStereoWidth() const;

private:
    //==============================================================================
    double sampleRate;
    
    float drive;      // 0 to 1
    float bitDepth;   // 1 to 16
    float stereoWidth; // 0 to 2
    
    // State for bit crushing
    float bitCrushPhase;
    float bitCrushLastSampleL;
    float bitCrushLastSampleR;
    
    // Apply drive (saturation) to a sample
    float applyDrive(float sample);
    
    // Apply bit crushing to a sample
    float applyBitCrush(float sample, int channel);
    
    // Apply stereo width to left/right channels
    void applyStereoWidth(float& left, float& right);
};