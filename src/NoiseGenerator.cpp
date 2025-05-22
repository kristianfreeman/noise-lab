#include "NoiseGenerator.h"

//==============================================================================
NoiseGenerator::NoiseGenerator()
    : currentNoiseType(WhiteNoise)
    , sampleRate(44100.0)
    , pinkNoiseCounter(0)
    , brownNoiseLastOutput(0.0f)
    , digitalCrunchBitDepth(8)
    , digitalCrunchSampleRate(22050.0f)
    , analogNoisePrevSample(0.0f)
    , analogNoiseFilterState(0.0f)
{
    // Initialize random number generator with a proper seed
    std::random_device rd;
    rng = std::mt19937(rd());
    distribution = std::uniform_real_distribution<float>(-1.0f, 1.0f);

    // Initialize pink noise values
    for (int i = 0; i < PINK_NOISE_NUM_STAGES; ++i)
    {
        pinkNoiseValues[i] = 0.0f;
    }
}

NoiseGenerator::~NoiseGenerator()
{
}

//==============================================================================
void NoiseGenerator::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    reset();
}

void NoiseGenerator::processBlock(juce::AudioBuffer<float>& buffer, int numSamples)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float noise = 0.0f;

            switch (currentNoiseType)
            {
                case WhiteNoise:
                    noise = generateWhiteNoise();
                    break;
                case PinkNoise:
                    noise = generatePinkNoise();
                    break;
                case BrownNoise:
                    noise = generateBrownNoise();
                    break;
                case DigitalCrunch:
                    noise = generateDigitalCrunch();
                    break;
                case AnalogSimulation:
                    noise = generateAnalogNoise();
                    break;
                default:
                    noise = 0.0f;
                    break;
            }

            channelData[sample] = noise;
        }
    }
}

void NoiseGenerator::reset()
{
    // Reset all noise generators
    brownNoiseLastOutput = 0.0f;
    analogNoisePrevSample = 0.0f;
    analogNoiseFilterState = 0.0f;

    for (int i = 0; i < PINK_NOISE_NUM_STAGES; ++i)
    {
        pinkNoiseValues[i] = 0.0f;
    }
}

//==============================================================================
void NoiseGenerator::setNoiseType(NoiseType type)
{
    currentNoiseType = type;
}

NoiseGenerator::NoiseType NoiseGenerator::getNoiseType() const
{
    return currentNoiseType;
}

//==============================================================================
float NoiseGenerator::generateWhiteNoise()
{
    return distribution(rng);
}

float NoiseGenerator::generatePinkNoise()
{
    float white = generateWhiteNoise();
    float pink = 0.0f;
    
    pinkNoiseCounter = (pinkNoiseCounter + 1) % 32768;
    
    for (int i = 0; i < PINK_NOISE_NUM_STAGES; i++) {
        if ((pinkNoiseCounter & (1 << i)) == 0) {
            pinkNoiseValues[i] = white;
        }
        pink += pinkNoiseValues[i];
    }
    
    // Normalize to same range as white noise (approximately)
    return pink * 0.125f;
}

float NoiseGenerator::generateBrownNoise()
{
    // Generate white noise sample
    float white = generateWhiteNoise();
    
    // Filter to create brown noise
    brownNoiseLastOutput = (brownNoiseLastOutput + (0.02f * white)) / 1.02f;
    
    // Normalize to prevent DC offset and scale to match other noise types
    return brownNoiseLastOutput * 3.5f;
}

float NoiseGenerator::generateDigitalCrunch()
{
    // Generate base noise
    float noise = generateWhiteNoise();
    
    // Apply bit reduction
    float scale = std::pow(2.0f, digitalCrunchBitDepth - 1) - 1.0f;
    noise = std::round(noise * scale) / scale;
    
    // Apply sample rate reduction (simple decimation)
    static float phase = 0.0f;
    static float lastSample = 0.0f;
    
    phase += digitalCrunchSampleRate / sampleRate;
    
    if (phase >= 1.0f) {
        phase -= 1.0f;
        lastSample = noise;
    }
    
    return lastSample;
}

float NoiseGenerator::generateAnalogNoise()
{
    // Generate base noise with subtle correlations
    float noise = 0.85f * analogNoisePrevSample + 0.15f * generateWhiteNoise();
    analogNoisePrevSample = noise;
    
    // Apply analog-style filtering
    // Simple one-pole lowpass filter to simulate circuit characteristics
    float cutoff = 7000.0f; // Hz
    float alpha = 1.0f / (1.0f + 2.0f * juce::MathConstants<float>::pi * (cutoff / float(sampleRate)));
    
    analogNoiseFilterState = alpha * analogNoiseFilterState + (1.0f - alpha) * noise;
    
    // Add subtle harmonic distortion
    float distorted = std::tanh(analogNoiseFilterState * 1.5f);
    
    // Mix in some higher frequency noise for transistor hiss
    float hiss = generateWhiteNoise() * 0.15f;
    hiss = juce::jlimit(-0.15f, 0.15f, hiss); // Limit the hiss amplitude
    
    return distorted + hiss;
}