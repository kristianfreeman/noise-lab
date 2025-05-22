#pragma once

#include <JuceHeader.h>
#include <random>

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
    NoiseGenerator();
    ~NoiseGenerator();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    void reset();

    //==============================================================================
    void setNoiseType(NoiseType type);
    NoiseType getNoiseType() const;

private:
    //==============================================================================
    // White noise generator
    float generateWhiteNoise();

    // Pink noise generator using Voss-McCartney algorithm
    float generatePinkNoise();

    // Brown noise generator
    float generateBrownNoise();

    // Digital crunch noise generator
    float generateDigitalCrunch();

    // Analog simulation noise generator
    float generateAnalogNoise();

    //==============================================================================
    NoiseType currentNoiseType;
    double sampleRate;

    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> distribution;

    // Pink noise state
    static constexpr int PINK_NOISE_NUM_STAGES = 8;
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