#include "FilterProcessor.h"

//==============================================================================
FilterProcessor::FilterProcessor()
    : sampleRate(44100.0)
    , currentFilterType(LowPass)
    , cutoffFrequency(1000.0f)  // Default: 1000 Hz
    , resonance(0.5f)           // Default: 0.5
    , modulationAmount(0.0f)
    , modulatedCutoff(1000.0f)
    , modulatedResonance(0.5f)
{
    reset();
}

FilterProcessor::~FilterProcessor()
{
}

//==============================================================================
void FilterProcessor::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    reset();
    updateFilter();
}

void FilterProcessor::processBlock(juce::AudioBuffer<float>& buffer, int numSamples)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            processSample(channelData[sample], channel);
        }
    }
}

void FilterProcessor::reset()
{
    // Reset state variable filter
    for (int channel = 0; channel < 2; ++channel)
    {
        svf.z1[channel] = 0.0f;
        svf.z2[channel] = 0.0f;
    }
}

//==============================================================================
void FilterProcessor::setFilterType(FilterType type)
{
    currentFilterType = type;
}

void FilterProcessor::setCutoffFrequency(float frequency)
{
    cutoffFrequency = juce::jlimit(20.0f, 20000.0f, frequency);
    updateFilter();
}

void FilterProcessor::setResonance(float newResonance)
{
    resonance = juce::jlimit(0.0f, 1.0f, newResonance);
    modulatedResonance = resonance; // Update modulated value too
    updateFilter();
}

FilterProcessor::FilterType FilterProcessor::getFilterType() const
{
    return currentFilterType;
}

float FilterProcessor::getCutoffFrequency() const
{
    return cutoffFrequency;
}

float FilterProcessor::getResonance() const
{
    return resonance;
}

//==============================================================================
void FilterProcessor::setModulationAmount(float amount)
{
    modulationAmount = amount;
}

float FilterProcessor::getModulationAmount() const
{
    return modulationAmount;
}

//==============================================================================
void FilterProcessor::applyModulation(float modulationValue)
{
    // Map the modulation value (-1 to 1) to a frequency multiplier
    // With modulationAmount = 1, the range will be cutoff/10 to cutoff*10
    // This gives a 10 octave modulation range at max depth
    
    const float modulationDepth = juce::jlimit(0.0f, 1.0f, std::abs(modulationAmount));
    const float octaveRange = 10.0f * modulationDepth;
    
    float multiplier = std::pow(2.0f, modulationValue * octaveRange);
    
    // Invert the multiplier if modulationAmount is negative
    if (modulationAmount < 0.0f)
        multiplier = 1.0f / multiplier;
    
    // Apply the modulation to the cutoff frequency
    modulatedCutoff = cutoffFrequency * multiplier;
    modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);
    
    updateFilter();
}

void FilterProcessor::applyResonanceModulation(float modulationValue)
{
    // Apply modulation to resonance (±50% range)
    float modulationRange = 0.5f;
    modulatedResonance = resonance * (1.0f + modulationValue * modulationRange);
    modulatedResonance = juce::jlimit(0.01f, 1.0f, modulatedResonance);
}

//==============================================================================
void FilterProcessor::updateFilter()
{
    // Nothing to update here, as the state variable filter
    // calculates coefficients per-sample in processSample
}

void FilterProcessor::processSample(float& sample, int channel)
{
    // State Variable Filter implementation
    // Based on the equations from the "Cookbook formulae for audio EQ biquad filter coefficients"
    // by Robert Bristow-Johnson
    
    // Convert cutoff from Hz to normalized frequency (0 to Nyquist)
    float f = modulatedCutoff / static_cast<float>(sampleRate);
    f = juce::jlimit(0.001f, 0.499f, f); // Limit to avoid instability
    
    // Calculate filter coefficients
    float k = std::tan(juce::MathConstants<float>::pi * f);
    float safeResonance = juce::jmax(0.1f, modulatedResonance); // Prevent instability
    float norm = 1.0f / (1.0f + k / safeResonance + k * k);
    float a0 = k * k * norm;
    float a1 = 2.0f * a0;
    float a2 = a0;
    float b1 = 2.0f * (k * k - 1.0f) * norm;
    float b2 = (1.0f - k / resonance + k * k) * norm;
    
    // Apply the filter (direct form II transposed)
    float input = sample;
    float output = a0 * input + svf.z1[channel];
    svf.z1[channel] = a1 * input - b1 * output + svf.z2[channel];
    svf.z2[channel] = a2 * input - b2 * output;
    
    // Output the appropriate type
    switch (currentFilterType)
    {
        case LowPass:
            sample = output;
            break;
        
        case BandPass:
            // Derive band-pass from the state variable filter
            sample = k / safeResonance * input - k / safeResonance * output;
            break;
        
        case HighPass:
            // Derive high-pass from the low-pass output
            sample = input - output;
            break;
            
        default:
            // Shouldn't happen, but default to low-pass
            sample = output;
            break;
    }
}