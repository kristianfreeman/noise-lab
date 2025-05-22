#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Utility functions for UI
namespace UIUtils
{
    // UI Colors
    const juce::Colour background = juce::Colour(0xFF1E1E1E);    // Dark gray background
    const juce::Colour highlight = juce::Colour(0xFF2C9AFF);     // Blue highlight
    const juce::Colour text = juce::Colour(0xFFE0E0E0);          // Light gray text
    const juce::Colour groupBg = juce::Colour(0xFF252525);       // Slightly lighter gray for groups
    const juce::Colour knobBg = juce::Colour(0xFF333333);        // Even lighter gray for knobs
    const juce::Colour warningColor = juce::Colour(0xFFFF5C5C);  // Red for warnings/clipping
    
    // UI Font
    juce::Font getFont(float size, bool bold = false)
    {
        if (bold)
            return juce::Font("Arial", size, juce::Font::bold);
        return juce::Font("Arial", size, juce::Font::plain);
    }
    
    // UI Text
    void drawText(juce::Graphics& g, const juce::String& text, juce::Rectangle<int> bounds, 
                 juce::Justification justification = juce::Justification::centred, bool isBold = false)
    {
        g.setColour(UIUtils::text);
        g.setFont(getFont(14.0f, isBold));
        g.drawText(text, bounds, justification);
    }
    
    // Draw knob with label
    void drawKnob(juce::Graphics& g, juce::Slider& slider, const juce::String& label)
    {
        // Draw background
        auto bounds = slider.getLocalBounds().reduced(2);
        g.setColour(knobBg);
        g.fillEllipse(bounds.toFloat());
        
        // Draw outer ring
        g.setColour(highlight);
        g.drawEllipse(bounds.toFloat(), 1.5f);
        
        // Draw value indicator line
        auto normValue = static_cast<float>(slider.valueToProportionOfLength(slider.getValue()));
        auto angle = juce::MathConstants<float>::pi * 1.5f + normValue * juce::MathConstants<float>::pi * 1.8f;
        
        auto center = bounds.getCentre().toFloat();
        auto radius = bounds.getWidth() * 0.4f;
        auto endX = center.x + radius * std::cos(angle);
        auto endY = center.y + radius * std::sin(angle);
        
        g.setColour(highlight);
        g.drawLine(center.x, center.y, endX, endY, 2.0f);
        
        // Draw label
        g.setColour(text);
        g.setFont(getFont(12.0f));
        g.drawText(label, bounds.removeFromBottom(20), juce::Justification::centred);
    }
    
    // Create styled slider
    std::unique_ptr<juce::Slider> createKnob()
    {
        auto knob = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, 
                                                juce::Slider::TextBoxBelow);
        
        // Improved knob aesthetics
        knob->setColour(juce::Slider::rotarySliderFillColourId, highlight.brighter(0.3f));
        knob->setColour(juce::Slider::rotarySliderOutlineColourId, knobBg.brighter(0.4f));
        knob->setColour(juce::Slider::thumbColourId, highlight);
        knob->setColour(juce::Slider::textBoxTextColourId, text);
        knob->setColour(juce::Slider::textBoxBackgroundColourId, background);
        knob->setColour(juce::Slider::textBoxOutlineColourId, background);
        knob->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 16);
        
        // Better rotation range and sensitivity
        knob->setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, 
                                 juce::MathConstants<float>::pi * 2.75f, true);
        knob->setMouseDragSensitivity(150);
        
        return knob;
    }
    
    // Create styled combo box
    std::unique_ptr<juce::ComboBox> createComboBox()
    {
        auto combo = std::make_unique<juce::ComboBox>();
        combo->setColour(juce::ComboBox::backgroundColourId, knobBg);
        combo->setColour(juce::ComboBox::textColourId, text);
        combo->setColour(juce::ComboBox::arrowColourId, highlight);
        combo->setColour(juce::ComboBox::outlineColourId, groupBg);
        return combo;
    }
    
    // Create styled button
    std::unique_ptr<juce::ToggleButton> createToggleButton(const juce::String& text)
    {
        auto button = std::make_unique<juce::ToggleButton>(text);
        button->setColour(juce::ToggleButton::textColourId, UIUtils::text);
        button->setColour(juce::ToggleButton::tickColourId, highlight);
        button->setColour(juce::ToggleButton::tickDisabledColourId, knobBg);
        return button;
    }
    
    // Draw a group background with title
    void drawGroupBackground(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title)
    {
        g.setColour(groupBg);
        g.fillRoundedRectangle(bounds.toFloat(), 6.0f);
        g.setColour(highlight);
        g.drawRoundedRectangle(bounds.toFloat(), 6.0f, 1.0f);
        
        // Draw title
        g.setFont(getFont(14.0f, true));
        g.setColour(text);
        g.drawText(title, bounds.removeFromTop(20), juce::Justification::centred);
    }
}

//==============================================================================
// NoiseTypeSelector Implementation
NoiseTypeSelector::NoiseTypeSelector(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : audioProcessor(p)
    , valueTreeState(apvts)
    , noiseTypeGroup("NoiseTypeGroup", "Noise Type")
{
    addAndMakeVisible(noiseTypeGroup);
    
    // Set up the noise type selector
    addAndMakeVisible(noiseTypeSelector);
    noiseTypeSelector.addItem("White Noise", 1);
    noiseTypeSelector.addItem("Pink Noise", 2);
    noiseTypeSelector.addItem("Brown Noise", 3);
    noiseTypeSelector.addItem("Digital Crunch", 4);
    noiseTypeSelector.addItem("Analog Simulation", 5);
    
    // Create attachment
    noiseTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        valueTreeState, "noiseType", noiseTypeSelector);
}

NoiseTypeSelector::~NoiseTypeSelector()
{
}

void NoiseTypeSelector::paint(juce::Graphics& g)
{
    UIUtils::drawGroupBackground(g, getLocalBounds(), "NOISE TYPE");
}

void NoiseTypeSelector::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    noiseTypeSelector.setBounds(bounds.reduced(5));
}

//==============================================================================
// TriggerModeSelector Implementation
TriggerModeSelector::TriggerModeSelector(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : audioProcessor(p)
    , valueTreeState(apvts)
    , triggerModeGroup("TriggerModeGroup", "Trigger Mode")
{
    addAndMakeVisible(triggerModeGroup);
    
    // Set up the trigger mode selector
    addAndMakeVisible(triggerModeSelector);
    triggerModeSelector.addItem("Free Run", 1);
    triggerModeSelector.addItem("MIDI Trigger", 2);
    triggerModeSelector.addItem("Host Sync", 3);
    triggerModeSelector.addItem("One-Shot", 4);
    
    // Create attachment
    triggerModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        valueTreeState, "triggerMode", triggerModeSelector);
}

TriggerModeSelector::~TriggerModeSelector()
{
}

void TriggerModeSelector::paint(juce::Graphics& g)
{
    UIUtils::drawGroupBackground(g, getLocalBounds(), "TRIGGER MODE");
}

void TriggerModeSelector::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    triggerModeSelector.setBounds(bounds.reduced(5));
}

//==============================================================================
// EnvelopeControls Implementation
EnvelopeControls::EnvelopeControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : audioProcessor(p)
    , valueTreeState(apvts)
    , envelopeGroup("EnvelopeGroup", "Envelope")
{
    addAndMakeVisible(envelopeGroup);
    
    // Set up the envelope controls
    attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    attackSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    attackSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    attackSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setColour(juce::Label::textColourId, UIUtils::text);
    attackLabel.setJustificationType(juce::Justification::centred);
    
    decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    decaySlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    decaySlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    decaySlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(decayLabel);
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.setColour(juce::Label::textColourId, UIUtils::text);
    decayLabel.setJustificationType(juce::Justification::centred);
    
    sustainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    sustainSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    sustainSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    sustainSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(sustainLabel);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setColour(juce::Label::textColourId, UIUtils::text);
    sustainLabel.setJustificationType(juce::Justification::centred);
    
    releaseSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    releaseSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    releaseSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    releaseSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setColour(juce::Label::textColourId, UIUtils::text);
    releaseLabel.setJustificationType(juce::Justification::centred);
    
    // Create attachments
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "attack", attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "decay", decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "sustain", sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "release", releaseSlider);
}

EnvelopeControls::~EnvelopeControls()
{
}

void EnvelopeControls::paint(juce::Graphics& g)
{
    UIUtils::drawGroupBackground(g, getLocalBounds(), "ENVELOPE");
}

void EnvelopeControls::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    
    int knobWidth = bounds.getWidth() / 4;
    
    // Attack
    auto attackArea = bounds.removeFromLeft(knobWidth);
    attackSlider.setBounds(attackArea.reduced(5).removeFromTop(attackArea.getHeight() - 20));
    attackLabel.setBounds(attackArea.removeFromBottom(20));
    
    // Decay
    auto decayArea = bounds.removeFromLeft(knobWidth);
    decaySlider.setBounds(decayArea.reduced(5).removeFromTop(decayArea.getHeight() - 20));
    decayLabel.setBounds(decayArea.removeFromBottom(20));
    
    // Sustain
    auto sustainArea = bounds.removeFromLeft(knobWidth);
    sustainSlider.setBounds(sustainArea.reduced(5).removeFromTop(sustainArea.getHeight() - 20));
    sustainLabel.setBounds(sustainArea.removeFromBottom(20));
    
    // Release
    auto releaseArea = bounds;
    releaseSlider.setBounds(releaseArea.reduced(5).removeFromTop(releaseArea.getHeight() - 20));
    releaseLabel.setBounds(releaseArea.removeFromBottom(20));
}

//==============================================================================
// ModulationControls Implementation
ModulationControls::ModulationControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : audioProcessor(p)
    , valueTreeState(apvts)
    , modulationGroup("ModulationGroup", "Modulation")
{
    addAndMakeVisible(modulationGroup);
    
    // Set up the modulation controls
    rateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    rateSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    rateSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    rateSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(rateSlider);
    addAndMakeVisible(rateLabel);
    rateLabel.setText("Rate", juce::dontSendNotification);
    rateLabel.setColour(juce::Label::textColourId, UIUtils::text);
    rateLabel.setJustificationType(juce::Justification::centred);
    
    depthSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    depthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    depthSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    depthSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    depthSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(depthSlider);
    addAndMakeVisible(depthLabel);
    depthLabel.setText("Depth", juce::dontSendNotification);
    depthLabel.setColour(juce::Label::textColourId, UIUtils::text);
    depthLabel.setJustificationType(juce::Justification::centred);
    
    syncToggle.setButtonText("Sync");
    syncToggle.setColour(juce::ToggleButton::textColourId, UIUtils::text);
    syncToggle.setColour(juce::ToggleButton::tickColourId, UIUtils::highlight);
    addAndMakeVisible(syncToggle);
    addAndMakeVisible(syncLabel);
    syncLabel.setText("Sync to Host", juce::dontSendNotification);
    syncLabel.setColour(juce::Label::textColourId, UIUtils::text);
    syncLabel.setJustificationType(juce::Justification::centred);
    
    targetSelector.addItem("Volume", 1);
    targetSelector.addItem("Filter Cutoff", 2);
    targetSelector.addItem("Filter Resonance", 3);
    targetSelector.addItem("Pitch", 4);
    targetSelector.setColour(juce::ComboBox::backgroundColourId, UIUtils::knobBg);
    targetSelector.setColour(juce::ComboBox::textColourId, UIUtils::text);
    targetSelector.setColour(juce::ComboBox::arrowColourId, UIUtils::highlight);
    addAndMakeVisible(targetSelector);
    addAndMakeVisible(targetLabel);
    targetLabel.setText("Target", juce::dontSendNotification);
    targetLabel.setColour(juce::Label::textColourId, UIUtils::text);
    targetLabel.setJustificationType(juce::Justification::centred);
    
    // Create attachments
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "lfoRate", rateSlider);
    depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "lfoDepth", depthSlider);
    syncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        valueTreeState, "lfoSync", syncToggle);
    targetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        valueTreeState, "lfoTarget", targetSelector);
}

ModulationControls::~ModulationControls()
{
}

void ModulationControls::paint(juce::Graphics& g)
{
    UIUtils::drawGroupBackground(g, getLocalBounds(), "MODULATION");
}

void ModulationControls::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    
    // Calculate control areas
    int controlWidth = bounds.getWidth() / 4;
    
    // Rate
    auto rateArea = bounds.removeFromLeft(controlWidth);
    rateSlider.setBounds(rateArea.reduced(5).removeFromTop(rateArea.getHeight() - 20));
    rateLabel.setBounds(rateArea.removeFromBottom(20));
    
    // Depth
    auto depthArea = bounds.removeFromLeft(controlWidth);
    depthSlider.setBounds(depthArea.reduced(5).removeFromTop(depthArea.getHeight() - 20));
    depthLabel.setBounds(depthArea.removeFromBottom(20));
    
    // Sync
    auto syncArea = bounds.removeFromLeft(controlWidth);
    syncToggle.setBounds(syncArea.reduced(5).removeFromTop(30));
    syncLabel.setBounds(syncArea.removeFromBottom(20));
    
    // Target
    auto targetArea = bounds;
    targetSelector.setBounds(targetArea.reduced(5).removeFromTop(30));
    targetLabel.setBounds(targetArea.removeFromBottom(20));
}

//==============================================================================
// FilterControls Implementation
FilterControls::FilterControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : audioProcessor(p)
    , valueTreeState(apvts)
    , filterGroup("FilterGroup", "Filter")
{
    addAndMakeVisible(filterGroup);
    
    // Set up the filter controls
    filterTypeSelector.addItem("Low Pass", 1);
    filterTypeSelector.addItem("Band Pass", 2);
    filterTypeSelector.addItem("High Pass", 3);
    filterTypeSelector.setColour(juce::ComboBox::backgroundColourId, UIUtils::knobBg);
    filterTypeSelector.setColour(juce::ComboBox::textColourId, UIUtils::text);
    filterTypeSelector.setColour(juce::ComboBox::arrowColourId, UIUtils::highlight);
    addAndMakeVisible(filterTypeSelector);
    addAndMakeVisible(typeLabel);
    typeLabel.setText("Type", juce::dontSendNotification);
    typeLabel.setColour(juce::Label::textColourId, UIUtils::text);
    typeLabel.setJustificationType(juce::Justification::centred);
    
    cutoffSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    cutoffSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    cutoffSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    cutoffSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(cutoffSlider);
    addAndMakeVisible(cutoffLabel);
    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffLabel.setColour(juce::Label::textColourId, UIUtils::text);
    cutoffLabel.setJustificationType(juce::Justification::centred);
    
    resonanceSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    resonanceSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    resonanceSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    resonanceSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(resonanceSlider);
    addAndMakeVisible(resonanceLabel);
    resonanceLabel.setText("Resonance", juce::dontSendNotification);
    resonanceLabel.setColour(juce::Label::textColourId, UIUtils::text);
    resonanceLabel.setJustificationType(juce::Justification::centred);
    
    // Create attachments
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        valueTreeState, "filterType", filterTypeSelector);
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "cutoff", cutoffSlider);
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "resonance", resonanceSlider);
}

FilterControls::~FilterControls()
{
}

void FilterControls::paint(juce::Graphics& g)
{
    UIUtils::drawGroupBackground(g, getLocalBounds(), "FILTER");
}

void FilterControls::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    
    // Calculate control areas
    int controlWidth = bounds.getWidth() / 3;
    
    // Type
    auto typeArea = bounds.removeFromLeft(controlWidth);
    filterTypeSelector.setBounds(typeArea.reduced(5).removeFromTop(30));
    typeLabel.setBounds(typeArea.removeFromBottom(20));
    
    // Cutoff
    auto cutoffArea = bounds.removeFromLeft(controlWidth);
    cutoffSlider.setBounds(cutoffArea.reduced(5).removeFromTop(cutoffArea.getHeight() - 20));
    cutoffLabel.setBounds(cutoffArea.removeFromBottom(20));
    
    // Resonance
    auto resonanceArea = bounds;
    resonanceSlider.setBounds(resonanceArea.reduced(5).removeFromTop(resonanceArea.getHeight() - 20));
    resonanceLabel.setBounds(resonanceArea.removeFromBottom(20));
}

//==============================================================================
// EffectsControls Implementation
EffectsControls::EffectsControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : audioProcessor(p)
    , valueTreeState(apvts)
    , effectsGroup("EffectsGroup", "Effects")
{
    addAndMakeVisible(effectsGroup);
    
    // Set up the effects controls
    driveSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    driveSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    driveSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    driveSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(driveLabel);
    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.setColour(juce::Label::textColourId, UIUtils::text);
    driveLabel.setJustificationType(juce::Justification::centred);
    
    bitcrushSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    bitcrushSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    bitcrushSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    bitcrushSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    bitcrushSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(bitcrushSlider);
    addAndMakeVisible(bitcrushLabel);
    bitcrushLabel.setText("Bitcrush", juce::dontSendNotification);
    bitcrushLabel.setColour(juce::Label::textColourId, UIUtils::text);
    bitcrushLabel.setJustificationType(juce::Justification::centred);
    
    widthSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    widthSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    widthSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    widthSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(widthSlider);
    addAndMakeVisible(widthLabel);
    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setColour(juce::Label::textColourId, UIUtils::text);
    widthLabel.setJustificationType(juce::Justification::centred);
    
    // Create attachments
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "drive", driveSlider);
    bitcrushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "bitcrush", bitcrushSlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "width", widthSlider);
}

EffectsControls::~EffectsControls()
{
}

void EffectsControls::paint(juce::Graphics& g)
{
    UIUtils::drawGroupBackground(g, getLocalBounds(), "EFFECTS");
}

void EffectsControls::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    
    // Calculate control areas
    int controlWidth = bounds.getWidth() / 3;
    
    // Drive
    auto driveArea = bounds.removeFromLeft(controlWidth);
    driveSlider.setBounds(driveArea.reduced(5).removeFromTop(driveArea.getHeight() - 20));
    driveLabel.setBounds(driveArea.removeFromBottom(20));
    
    // Bitcrush
    auto bitcrushArea = bounds.removeFromLeft(controlWidth);
    bitcrushSlider.setBounds(bitcrushArea.reduced(5).removeFromTop(bitcrushArea.getHeight() - 20));
    bitcrushLabel.setBounds(bitcrushArea.removeFromBottom(20));
    
    // Width
    auto widthArea = bounds;
    widthSlider.setBounds(widthArea.reduced(5).removeFromTop(widthArea.getHeight() - 20));
    widthLabel.setBounds(widthArea.removeFromBottom(20));
}

//==============================================================================
// GlobalControls Implementation
GlobalControls::GlobalControls(NoiseLabAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : audioProcessor(p)
    , valueTreeState(apvts)
    , globalGroup("GlobalGroup", "Global")
{
    addAndMakeVisible(globalGroup);
    
    // Set up the global controls
    outputSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    outputSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    outputSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    outputSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(outputSlider);
    addAndMakeVisible(outputLabel);
    outputLabel.setText("Output", juce::dontSendNotification);
    outputLabel.setColour(juce::Label::textColourId, UIUtils::text);
    outputLabel.setJustificationType(juce::Justification::centred);
    
    dryWetSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    dryWetSlider.setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlight);
    dryWetSlider.setColour(juce::Slider::textBoxTextColourId, UIUtils::text);
    dryWetSlider.setColour(juce::Slider::textBoxBackgroundColourId, UIUtils::background);
    addAndMakeVisible(dryWetSlider);
    addAndMakeVisible(dryWetLabel);
    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.setColour(juce::Label::textColourId, UIUtils::text);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    
    // Create attachments
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "output", outputSlider);
    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "dryWet", dryWetSlider);
}

GlobalControls::~GlobalControls()
{
}

void GlobalControls::paint(juce::Graphics& g)
{
    UIUtils::drawGroupBackground(g, getLocalBounds(), "OUTPUT");
}

void GlobalControls::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    
    // Calculate control areas
    int controlWidth = bounds.getWidth() / 2;
    
    // Output
    auto outputArea = bounds.removeFromLeft(controlWidth);
    outputSlider.setBounds(outputArea.reduced(5).removeFromTop(outputArea.getHeight() - 20));
    outputLabel.setBounds(outputArea.removeFromBottom(20));
    
    // Dry/Wet
    auto dryWetArea = bounds;
    dryWetSlider.setBounds(dryWetArea.reduced(5).removeFromTop(dryWetArea.getHeight() - 20));
    dryWetLabel.setBounds(dryWetArea.removeFromBottom(20));
}

//==============================================================================
// NoiseLabAudioProcessorEditor Implementation
NoiseLabAudioProcessorEditor::NoiseLabAudioProcessorEditor(NoiseLabAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
    , noiseTypeSelector(p, p.apvts)
    , triggerModeSelector(p, p.apvts)
    , envelopeControls(p, p.apvts)
    , modulationControls(p, p.apvts)
    , filterControls(p, p.apvts)
    , effectsControls(p, p.apvts)
    , globalControls(p, p.apvts)
{
    // Load logo image
    // logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
    
    // Add all components
    addAndMakeVisible(noiseTypeSelector);
    addAndMakeVisible(triggerModeSelector);
    addAndMakeVisible(envelopeControls);
    addAndMakeVisible(modulationControls);
    addAndMakeVisible(filterControls);
    addAndMakeVisible(effectsControls);
    addAndMakeVisible(globalControls);
    
    // Set window size - increased width for better layout
    setSize(900, 600);
}

NoiseLabAudioProcessorEditor::~NoiseLabAudioProcessorEditor()
{
}

//==============================================================================
void NoiseLabAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(UIUtils::background);
    
    // Draw plugin header
    g.setColour(UIUtils::highlight);
    g.fillRect(0, 0, getWidth(), 40);
    
    // Draw logo
    // if (logoImage.isValid())
    // {
    //     g.drawImage(logoImage, 10, 5, 30, 30, 0, 0, logoImage.getWidth(), logoImage.getHeight());
    // }
    
    // Draw title
    g.setFont(UIUtils::getFont(22.0f, true));
    g.setColour(UIUtils::background);
    g.drawText("NOISE LAB", 50, 0, getWidth() - 60, 40, juce::Justification::centredLeft);
    
    // Draw version
    g.setFont(UIUtils::getFont(12.0f));
    g.drawText("v1.0.0", getWidth() - 60, 0, 50, 40, juce::Justification::centredRight);
}

void NoiseLabAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(40); // Header space
    
    // Layout calculations - improved spacing for wider window
    const int margin = 10;
    bounds.reduce(margin, margin);
    
    const int columnWidth = (bounds.getWidth() - 2 * margin) / 3;
    const int rowHeight = bounds.getHeight() / 3;
    
    // Left column
    auto leftColumn = bounds.removeFromLeft(columnWidth);
    leftColumn.removeFromRight(margin / 2); // Add spacing between columns
    
    // Noise Type selector (top left)
    noiseTypeSelector.setBounds(leftColumn.removeFromTop(rowHeight));
    
    // Trigger Mode selector (middle left)
    triggerModeSelector.setBounds(leftColumn.removeFromTop(rowHeight));
    
    // Global controls (bottom left)
    globalControls.setBounds(leftColumn);
    
    // Middle column
    auto middleColumn = bounds.removeFromLeft(columnWidth);
    middleColumn.removeFromLeft(margin / 2);
    middleColumn.removeFromRight(margin / 2);
    
    // Envelope controls (top and middle)
    envelopeControls.setBounds(middleColumn.removeFromTop(rowHeight * 2));
    
    // Filter controls (bottom middle)
    filterControls.setBounds(middleColumn);
    
    // Right column
    auto rightColumn = bounds;
    rightColumn.removeFromLeft(margin / 2);
    
    // Modulation controls (top right)
    modulationControls.setBounds(rightColumn.removeFromTop(rowHeight * 1.5));
    
    // Effects controls (bottom right)
    effectsControls.setBounds(rightColumn);
}

