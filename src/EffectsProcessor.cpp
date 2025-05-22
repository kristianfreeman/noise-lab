#include "EffectsProcessor.h"

//==============================================================================
EffectsProcessor::EffectsProcessor()
    : sampleRate(44100.0)
    , drive(0.0f)         // Default: 0%
    , bitDepth(16.0f)     // Default: 16-bit (no reduction)
    , stereoWidth(1.0f)   // Default: 100% (normal stereo)
    , bitCrushPhase(0.0f)
    , bitCrushLastSampleL(0.0f)
    , bitCrushLastSampleR(0.0f)
{
}

EffectsProcessor::~EffectsProcessor()
{
}

//==============================================================================
void EffectsProcessor::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    reset();
}

void EffectsProcessor::processBlock(juce::AudioBuffer<float>& buffer, int numSamples)
{
    // Stereo width requires at least 2 channels
    const bool stereo = buffer.getNumChannels() >= 2;
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get left channel sample
        float left = buffer.getSample(0, sample);
        
        // Apply drive
        left = applyDrive(left);
        
        // Apply bit crush
        left = applyBitCrush(left, 0);
        
        // Handle stereo if available
        if (stereo)
        {
            float right = buffer.getSample(1, sample);
            right = applyDrive(right);
            right = applyBitCrush(right, 1);
            
            // Apply stereo width
            applyStereoWidth(left, right);
            
            // Write back to buffer
            buffer.setSample(0, sample, left);
            buffer.setSample(1, sample, right);
        }
        else
        {
            // Mono case
            buffer.setSample(0, sample, left);
        }
    }
}

void EffectsProcessor::reset()
{
    bitCrushPhase = 0.0f;
    bitCrushLastSampleL = 0.0f;
    bitCrushLastSampleR = 0.0f;
}

//==============================================================================
void EffectsProcessor::setDrive(float newDrive)
{
    drive = juce::jlimit(0.0f, 1.0f, newDrive);
}

void EffectsProcessor::setBitcrush(float newBitDepth)
{
    bitDepth = juce::jlimit(1.0f, 16.0f, newBitDepth);
}

void EffectsProcessor::setStereoWidth(float width)
{
    stereoWidth = juce::jlimit(0.0f, 2.0f, width);
}

float EffectsProcessor::getDrive() const
{
    return drive;
}

float EffectsProcessor::getBitcrush() const
{
    return bitDepth;
}

float EffectsProcessor::getStereoWidth() const
{
    return stereoWidth;
}

//==============================================================================
float EffectsProcessor::applyDrive(float sample)
{
    // Simple tanh distortion with variable drive
    if (drive <= 0.0f)
        return sample;
    
    // Scale drive from 0-1 to a more useful range
    const float driveAmount = 1.0f + drive * 9.0f; // 1 to 10
    
    // Apply soft clipping with variable drive
    float driven = std::tanh(sample * driveAmount);
    
    // Compensate for volume increase when adding drive
    driven /= (0.5f * drive + 0.5f);
    
    return driven;
}

float EffectsProcessor::applyBitCrush(float sample, int channel)
{
    // If bit depth is maximum, no effect
    if (bitDepth >= 16.0f)
        return sample;
    
    // Calculate the number of levels based on bit depth
    const int levels = static_cast<int>(std::pow(2.0f, bitDepth) - 1.0f);
    
    // Apply bit reduction
    const float crushedSample = std::round(sample * levels) / static_cast<float>(levels);
    
    // At very low bit depths, also apply sample rate reduction for a more digital sound
    if (bitDepth <= 8.0f)
    {
        // Map 1-8 bit depth to sample rate reduction factor 
        // (lower bit depth = more sample rate reduction)
        const float srReductionFactor = 0.5f * (8.0f - bitDepth) / 8.0f; // 0 to 0.5
        
        // Calculate phase increment for sample rate reduction
        const float phaseIncrement = 1.0f / (1.0f + srReductionFactor * 40.0f); // 1 to 1/20
        
        // Update phase
        bitCrushPhase += phaseIncrement;
        
        // Check if we need to update the output sample
        if (bitCrushPhase >= 1.0f)
        {
            bitCrushPhase -= 1.0f;
            
            // Update the held sample
            if (channel == 0)
                bitCrushLastSampleL = crushedSample;
            else
                bitCrushLastSampleR = crushedSample;
        }
        
        // Return the held sample
        return (channel == 0) ? bitCrushLastSampleL : bitCrushLastSampleR;
    }
    
    return crushedSample;
}

void EffectsProcessor::applyStereoWidth(float& left, float& right)
{
    // If stereo width is normal (1.0), no effect
    if (std::abs(stereoWidth - 1.0f) < 0.01f)
        return;
    
    // Calculate mid and side components
    float mid = (left + right) * 0.5f;
    float side = (left - right) * 0.5f;
    
    // Apply width to side component
    side *= stereoWidth;
    
    // Convert back to left/right
    left = mid + side;
    right = mid - side;
}