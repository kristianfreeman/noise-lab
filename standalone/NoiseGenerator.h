#pragma once

#include <cmath>
#include <random>
#include <iostream>

//==============================================================================
/**
 * The main noise generator class that produces various types of noise.
 */
class NoiseGenerator
{
public:
    //==============================================================================
    enum NoiseType
    {
        WhiteNoise = 0,
        PinkNoise,
        BrownNoise,
        DigitalCrunch,
        AnalogSimulation,
        NumNoiseTypes
    };

    //==============================================================================
    NoiseGenerator()
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

    //==============================================================================
    void setSampleRate(double newSampleRate)
    {
        sampleRate = newSampleRate;
        reset();
    }

    void reset()
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
    void setNoiseType(NoiseType type)
    {
        currentNoiseType = type;
    }

    NoiseType getNoiseType() const
    {
        return currentNoiseType;
    }

    //==============================================================================
    float generateSample()
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

        return noise;
    }

private:
    //==============================================================================
    // White noise generator
    float generateWhiteNoise()
    {
        return distribution(rng);
    }

    // Pink noise generator using Voss-McCartney algorithm
    float generatePinkNoise()
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

    // Brown noise generator
    float generateBrownNoise()
    {
        // Generate white noise sample
        float white = generateWhiteNoise();
        
        // Filter to create brown noise
        brownNoiseLastOutput = (brownNoiseLastOutput + (0.02f * white)) / 1.02f;
        
        // Normalize to prevent DC offset and scale to match other noise types
        return brownNoiseLastOutput * 3.5f;
    }

    // Digital crunch noise generator
    float generateDigitalCrunch()
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

    // Analog simulation noise generator
    float generateAnalogNoise()
    {
        // Generate base noise with subtle correlations
        float noise = 0.85f * analogNoisePrevSample + 0.15f * generateWhiteNoise();
        analogNoisePrevSample = noise;
        
        // Apply analog-style filtering
        // Simple one-pole lowpass filter to simulate circuit characteristics
        float cutoff = 7000.0f; // Hz
        float alpha = 1.0f / (1.0f + 2.0f * 3.14159f * (cutoff / float(sampleRate)));
        
        analogNoiseFilterState = alpha * analogNoiseFilterState + (1.0f - alpha) * noise;
        
        // Add subtle harmonic distortion
        float distorted = std::tanh(analogNoiseFilterState * 1.5f);
        
        // Mix in some higher frequency noise for transistor hiss
        float hiss = generateWhiteNoise() * 0.15f;
        hiss = std::max(-0.15f, std::min(0.15f, hiss)); // Limit the hiss amplitude
        
        return distorted + hiss;
    }

    //==============================================================================
    NoiseType currentNoiseType;
    double sampleRate;

    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> distribution;

    // Pink noise state
    static const int PINK_NOISE_NUM_STAGES = 8;
    float pinkNoiseValues[PINK_NOISE_NUM_STAGES];
    int pinkNoiseCounter;

    // Brown noise state
    float brownNoiseLastOutput;

    // Digital crunch state
    int digitalCrunchBitDepth;
    float digitalCrunchSampleRate;

    // Analog simulation state
    float analogNoisePrevSample;
    float analogNoiseFilterState;
};