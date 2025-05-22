#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Filter processor for the noise generator.
 */
class FilterProcessor
{
public:
    //==============================================================================
    enum FilterType
    {
        LowPass = 0,
        BandPass,
        HighPass,
        NumFilterTypes
    };

    //==============================================================================
    FilterProcessor();
    ~FilterProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    void reset();

    //==============================================================================
    void setFilterType(FilterType type);
    void setCutoffFrequency(float frequency);
    void setResonance(float resonance);
    
    FilterType getFilterType() const;
    float getCutoffFrequency() const;
    float getResonance() const;
    
    //==============================================================================
    void setModulationAmount(float amount);
    float getModulationAmount() const;
    
    //==============================================================================
    void applyModulation(float modulationValue);
    void applyResonanceModulation(float modulationValue);

private:
    //==============================================================================
    double sampleRate;
    
    FilterType currentFilterType;
    float cutoffFrequency;
    float resonance;
    
    float modulationAmount;
    float modulatedCutoff;
    float modulatedResonance;
    
    // State variable filter implementation
    struct SVFilter
    {
        float z1[2];
        float z2[2];
    };
    
    SVFilter svf;
    
    //==============================================================================
    void updateFilter();
    void processSample(float& sample, int channel);
};