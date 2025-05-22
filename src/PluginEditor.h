#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 * Custom UI component for the noise type selector
 */
class NoiseTypeSelector : public juce::Component
{
public:
    NoiseTypeSelector(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~NoiseTypeSelector() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    NoiseLabAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::GroupComponent noiseTypeGroup;
    juce::ComboBox noiseTypeSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> noiseTypeAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseTypeSelector)
};

//==============================================================================
/**
 * Custom UI component for the trigger mode selector
 */
class TriggerModeSelector : public juce::Component
{
public:
    TriggerModeSelector(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~TriggerModeSelector() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    NoiseLabAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::GroupComponent triggerModeGroup;
    juce::ComboBox triggerModeSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> triggerModeAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerModeSelector)
};

//==============================================================================
/**
 * Custom UI component for the envelope controls
 */
class EnvelopeControls : public juce::Component
{
public:
    EnvelopeControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~EnvelopeControls() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    NoiseLabAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::GroupComponent envelopeGroup;
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeControls)
};

//==============================================================================
/**
 * Custom UI component for the modulation controls
 */
class ModulationControls : public juce::Component
{
public:
    ModulationControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~ModulationControls() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    NoiseLabAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::GroupComponent modulationGroup;
    juce::Slider rateSlider, depthSlider;
    juce::ToggleButton syncToggle;
    juce::ComboBox targetSelector;
    juce::Label rateLabel, depthLabel, syncLabel, targetLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> targetAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationControls)
};

//==============================================================================
/**
 * Custom UI component for the filter controls
 */
class FilterControls : public juce::Component
{
public:
    FilterControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~FilterControls() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    NoiseLabAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::GroupComponent filterGroup;
    juce::ComboBox filterTypeSelector;
    juce::Slider cutoffSlider, resonanceSlider;
    juce::Label typeLabel, cutoffLabel, resonanceLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterControls)
};

//==============================================================================
/**
 * Custom UI component for the effects controls
 */
class EffectsControls : public juce::Component
{
public:
    EffectsControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~EffectsControls() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    NoiseLabAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::GroupComponent effectsGroup;
    juce::Slider driveSlider, bitcrushSlider, widthSlider;
    juce::Label driveLabel, bitcrushLabel, widthLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitcrushAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectsControls)
};

//==============================================================================
/**
 * Custom UI component for the global controls
 */
class GlobalControls : public juce::Component
{
public:
    GlobalControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~GlobalControls() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    NoiseLabAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::GroupComponent globalGroup;
    juce::Slider outputSlider, dryWetSlider;
    juce::Label outputLabel, dryWetLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlobalControls)
};

//==============================================================================
/**
 * Main editor for the Noise Lab plugin.
 */
class NoiseLabAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    NoiseLabAudioProcessorEditor(NoiseLabAudioProcessor&);
    ~NoiseLabAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Reference to the processor
    NoiseLabAudioProcessor& audioProcessor;
    
    // UI components
    NoiseTypeSelector noiseTypeSelector;
    TriggerModeSelector triggerModeSelector;
    EnvelopeControls envelopeControls;
    ModulationControls modulationControls;
    FilterControls filterControls;
    EffectsControls effectsControls;
    GlobalControls globalControls;
    
    // Logo and branding
    juce::Image logoImage;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseLabAudioProcessorEditor)
};