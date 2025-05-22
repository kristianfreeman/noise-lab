#pragma once

#include <JuceHeader.h>
#include "NoiseGenerator.h"
#include "EnvelopeGenerator.h"
#include "LFOGenerator.h"
#include "FilterProcessor.h"
#include "EffectsProcessor.h"

//==============================================================================
/**
 * Main audio processor for the Noise Lab plugin.
 */
class NoiseLabAudioProcessor : public juce::AudioProcessor,
                               public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    NoiseLabAudioProcessor();
    ~NoiseLabAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    // Audio processor value tree state
    juce::AudioProcessorValueTreeState apvts;

private:
    // Parameter setup
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    // Processors
    NoiseGenerator noiseGenerator;
    EnvelopeGenerator envelopeGenerator;
    LFOGenerator lfoGenerator;
    FilterProcessor filterProcessor;
    EffectsProcessor effectsProcessor;

    // Trigger mode
    enum TriggerMode {
        FREE_RUN,
        MIDI_TRIGGER,
        HOST_SYNC,
        ONE_SHOT
    };
    TriggerMode currentTriggerMode;
    
    // MIDI handling
    struct MidiNote {
        int noteNumber;
        int velocity;
        bool isActive;
    };
    std::vector<MidiNote> activeNotes;
    
    // Host sync
    bool isPlaying;
    double bpm;
    double ppqPosition;
    
    // Output controls
    float outputLevel;
    float dryWetMix;
    
    // Last sample rate
    double currentSampleRate;
    
    // Buffer for dry/wet processing
    juce::AudioBuffer<float> dryBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseLabAudioProcessor)
};