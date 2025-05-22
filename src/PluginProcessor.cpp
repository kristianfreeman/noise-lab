#include "PluginProcessor.h"
#ifndef JUCE_HEADLESS
#include "PluginEditor.h"
#endif

//==============================================================================
NoiseLabAudioProcessor::NoiseLabAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameters())
    , currentTriggerMode(MIDI_TRIGGER)
    , isPlaying(false)
    , bpm(120.0)
    , ppqPosition(0.0)
    , outputLevel(1.0f)
    , dryWetMix(1.0f)
    , currentSampleRate(44100.0)
{
    // Add parameter listeners
    apvts.addParameterListener("noiseType", this);
    apvts.addParameterListener("triggerMode", this);
    apvts.addParameterListener("attack", this);
    apvts.addParameterListener("decay", this);
    apvts.addParameterListener("sustain", this);
    apvts.addParameterListener("release", this);
    apvts.addParameterListener("lfoRate", this);
    apvts.addParameterListener("lfoDepth", this);
    apvts.addParameterListener("lfoSync", this);
    apvts.addParameterListener("lfoTarget", this);
    apvts.addParameterListener("filterType", this);
    apvts.addParameterListener("cutoff", this);
    apvts.addParameterListener("resonance", this);
    apvts.addParameterListener("drive", this);
    apvts.addParameterListener("bitcrush", this);
    apvts.addParameterListener("width", this);
    apvts.addParameterListener("output", this);
    apvts.addParameterListener("dryWet", this);
    
    // Initialize all parameters
    parameterChanged("noiseType", *apvts.getRawParameterValue("noiseType"));
    parameterChanged("triggerMode", *apvts.getRawParameterValue("triggerMode"));
    parameterChanged("attack", *apvts.getRawParameterValue("attack"));
    parameterChanged("decay", *apvts.getRawParameterValue("decay"));
    parameterChanged("sustain", *apvts.getRawParameterValue("sustain"));
    parameterChanged("release", *apvts.getRawParameterValue("release"));
    parameterChanged("lfoRate", *apvts.getRawParameterValue("lfoRate"));
    parameterChanged("lfoDepth", *apvts.getRawParameterValue("lfoDepth"));
    parameterChanged("lfoSync", *apvts.getRawParameterValue("lfoSync"));
    parameterChanged("lfoTarget", *apvts.getRawParameterValue("lfoTarget"));
    parameterChanged("filterType", *apvts.getRawParameterValue("filterType"));
    parameterChanged("cutoff", *apvts.getRawParameterValue("cutoff"));
    parameterChanged("resonance", *apvts.getRawParameterValue("resonance"));
    parameterChanged("drive", *apvts.getRawParameterValue("drive"));
    parameterChanged("bitcrush", *apvts.getRawParameterValue("bitcrush"));
    parameterChanged("width", *apvts.getRawParameterValue("width"));
    parameterChanged("output", *apvts.getRawParameterValue("output"));
    parameterChanged("dryWet", *apvts.getRawParameterValue("dryWet"));
}

NoiseLabAudioProcessor::~NoiseLabAudioProcessor()
{
    // Remove parameter listeners
    apvts.removeParameterListener("noiseType", this);
    apvts.removeParameterListener("triggerMode", this);
    apvts.removeParameterListener("attack", this);
    apvts.removeParameterListener("decay", this);
    apvts.removeParameterListener("sustain", this);
    apvts.removeParameterListener("release", this);
    apvts.removeParameterListener("lfoRate", this);
    apvts.removeParameterListener("lfoDepth", this);
    apvts.removeParameterListener("lfoSync", this);
    apvts.removeParameterListener("lfoTarget", this);
    apvts.removeParameterListener("filterType", this);
    apvts.removeParameterListener("cutoff", this);
    apvts.removeParameterListener("resonance", this);
    apvts.removeParameterListener("drive", this);
    apvts.removeParameterListener("bitcrush", this);
    apvts.removeParameterListener("width", this);
    apvts.removeParameterListener("output", this);
    apvts.removeParameterListener("dryWet", this);
}

//==============================================================================
void NoiseLabAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    // Prepare all processors
    noiseGenerator.prepareToPlay(sampleRate, samplesPerBlock);
    envelopeGenerator.prepareToPlay(sampleRate, samplesPerBlock);
    lfoGenerator.prepareToPlay(sampleRate, samplesPerBlock);
    filterProcessor.prepareToPlay(sampleRate, samplesPerBlock);
    effectsProcessor.prepareToPlay(sampleRate, samplesPerBlock);
    
    // Clear any leftover MIDI notes
    activeNotes.clear();
    
    // Initialize dry buffer for dry/wet processing
    dryBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
}

void NoiseLabAudioProcessor::releaseResources()
{
    // Release all processors
    noiseGenerator.reset();
    envelopeGenerator.reset();
    lfoGenerator.reset();
    filterProcessor.reset();
    effectsProcessor.reset();
}

bool NoiseLabAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Only mono or stereo is supported
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // Input must match output
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

void NoiseLabAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Update playback position from host
    auto playHead = getPlayHead();
    if (playHead != nullptr)
    {
        // Use JUCE AudioPlayHead API
        juce::AudioPlayHead::CurrentPositionInfo positionInfo;
        if (playHead->getCurrentPosition(positionInfo))
        {
            isPlaying = positionInfo.isPlaying;
            bpm = positionInfo.bpm;
            ppqPosition = positionInfo.ppqPosition;
            
            // Update LFO with host timing info
            lfoGenerator.setHostBPM(bpm);
            lfoGenerator.setHostPPQPosition(ppqPosition);
        }
    }
    
    // Process MIDI messages
    for (const auto& metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        
        if (message.isNoteOn())
        {
            // Add to active notes
            MidiNote note;
            note.noteNumber = message.getNoteNumber();
            note.velocity = static_cast<int>(message.getVelocity());
            note.isActive = true;
            
            activeNotes.push_back(note);
            
            // Trigger envelope
            float velocityAsFloat = static_cast<float>(note.velocity) / 127.0f;
            envelopeGenerator.noteOn(note.noteNumber, velocityAsFloat);
        }
        else if (message.isNoteOff())
        {
            // Remove from active notes
            for (auto& note : activeNotes)
            {
                if (note.noteNumber == message.getNoteNumber())
                {
                    note.isActive = false;
                }
            }
            
            // If no active notes left, trigger envelope release
            bool anyActive = false;
            for (const auto& note : activeNotes)
            {
                if (note.isActive)
                {
                    anyActive = true;
                    break;
                }
            }
            
            if (!anyActive)
            {
                envelopeGenerator.noteOff(message.getNoteNumber());
            }
        }
        else if (message.isAllNotesOff())
        {
            // Clear all notes
            activeNotes.clear();
            envelopeGenerator.reset();
        }
    }
    
    // Make a copy of the input for dry/wet mixing if needed
    if (dryWetMix < 1.0f)
    {
        dryBuffer.makeCopyOf(buffer);
    }
    else
    {
        // If not using input (100% wet), clear the buffer
        buffer.clear();
    }
    
    // Generate noise
    noiseGenerator.processBlock(buffer, buffer.getNumSamples());
    
    // Apply envelope (always process, but behavior depends on trigger mode)
    envelopeGenerator.processBlock(buffer, buffer.getNumSamples());
    
    // Process LFO and apply modulation
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float lfoValue = lfoGenerator.processSample();
        
        // Apply LFO to target parameter
        switch (lfoGenerator.getTarget())
        {
            case LFOGenerator::Volume:
                // Apply directly to each sample
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    float* channelData = buffer.getWritePointer(channel);
                    channelData[i] *= 0.5f + 0.5f * (1.0f + lfoValue);
                }
                break;
                
            case LFOGenerator::FilterCutoff:
                // Apply to filter cutoff
                filterProcessor.applyModulation(lfoValue);
                break;
                
            case LFOGenerator::FilterResonance:
                // Apply to filter resonance (but don't change the actual parameter)
                // Instead, apply modulation to the filter directly
                filterProcessor.applyResonanceModulation(lfoValue);
                break;
                
            case LFOGenerator::Pitch:
                // For noise, "pitch" modulation is not directly applicable
                // Instead, we could alter the filter to create a pitch-like effect
                // This is a simplification
                filterProcessor.applyModulation(lfoValue * 0.5f);
                break;
                
            case LFOGenerator::NumTargets:
                // This is an enum bound and should not be used directly
                break;
                
            default:
                // Handle other cases or future additions
                break;
        }
    }
    
    // Apply filter
    filterProcessor.processBlock(buffer, buffer.getNumSamples());
    
    // Apply effects (drive, bitcrush, stereo width)
    effectsProcessor.processBlock(buffer, buffer.getNumSamples());
    
    // Apply output level
    buffer.applyGain(juce::Decibels::decibelsToGain(outputLevel));
    
    // Apply dry/wet mix if needed
    if (dryWetMix < 1.0f)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float* data = buffer.getWritePointer(channel);
            const float* dryData = dryBuffer.getReadPointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                data[sample] = dryData[sample] * (1.0f - dryWetMix) + data[sample] * dryWetMix;
            }
        }
    }
}

//==============================================================================
juce::AudioProcessorEditor* NoiseLabAudioProcessor::createEditor()
{
#if JUCE_HEADLESS
    return nullptr;
#else
    return new NoiseLabAudioProcessorEditor(*this);
#endif
}

bool NoiseLabAudioProcessor::hasEditor() const
{
#if JUCE_HEADLESS
    return false;
#else
    return true;
#endif
}

//==============================================================================
const juce::String NoiseLabAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NoiseLabAudioProcessor::acceptsMidi() const
{
    return true;
}

bool NoiseLabAudioProcessor::producesMidi() const
{
    return false;
}

bool NoiseLabAudioProcessor::isMidiEffect() const
{
    return false;
}

double NoiseLabAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
int NoiseLabAudioProcessor::getNumPrograms()
{
    return 1;  // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NoiseLabAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NoiseLabAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String NoiseLabAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void NoiseLabAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void NoiseLabAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Store parameters
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NoiseLabAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore parameters
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

//==============================================================================
void NoiseLabAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "noiseType")
    {
        noiseGenerator.setNoiseType(static_cast<NoiseGenerator::NoiseType>(static_cast<int>(newValue)));
    }
    else if (parameterID == "triggerMode")
    {
        currentTriggerMode = static_cast<TriggerMode>(static_cast<int>(newValue));
        
        // Configure envelope based on trigger mode
        envelopeGenerator.setOneShot(currentTriggerMode == ONE_SHOT);
        
        // For free-run mode, trigger envelope immediately
        if (currentTriggerMode == FREE_RUN)
        {
            envelopeGenerator.noteOn(60, 1.0f); // Trigger with full velocity
        }
        else if (currentTriggerMode == MIDI_TRIGGER)
        {
            // Reset envelope for MIDI mode - it will only trigger on MIDI notes
            envelopeGenerator.reset();
        }
    }
    else if (parameterID == "attack")
    {
        envelopeGenerator.setParameters(
            newValue,
            envelopeGenerator.getDecayTime(),
            envelopeGenerator.getSustainLevel(),
            envelopeGenerator.getReleaseTime()
        );
    }
    else if (parameterID == "decay")
    {
        envelopeGenerator.setParameters(
            envelopeGenerator.getAttackTime(),
            newValue,
            envelopeGenerator.getSustainLevel(),
            envelopeGenerator.getReleaseTime()
        );
    }
    else if (parameterID == "sustain")
    {
        envelopeGenerator.setParameters(
            envelopeGenerator.getAttackTime(),
            envelopeGenerator.getDecayTime(),
            newValue,
            envelopeGenerator.getReleaseTime()
        );
    }
    else if (parameterID == "release")
    {
        envelopeGenerator.setParameters(
            envelopeGenerator.getAttackTime(),
            envelopeGenerator.getDecayTime(),
            envelopeGenerator.getSustainLevel(),
            newValue
        );
    }
    else if (parameterID == "lfoRate")
    {
        lfoGenerator.setRate(newValue);
    }
    else if (parameterID == "lfoDepth")
    {
        lfoGenerator.setDepth(newValue);
    }
    else if (parameterID == "lfoSync")
    {
        lfoGenerator.setSyncToHost(newValue >= 0.5f);
    }
    else if (parameterID == "lfoTarget")
    {
        lfoGenerator.setTarget(static_cast<LFOGenerator::LFOTarget>(static_cast<int>(newValue)));
    }
    else if (parameterID == "filterType")
    {
        filterProcessor.setFilterType(static_cast<FilterProcessor::FilterType>(static_cast<int>(newValue)));
    }
    else if (parameterID == "cutoff")
    {
        filterProcessor.setCutoffFrequency(newValue);
    }
    else if (parameterID == "resonance")
    {
        filterProcessor.setResonance(newValue);
    }
    else if (parameterID == "drive")
    {
        effectsProcessor.setDrive(newValue);
    }
    else if (parameterID == "bitcrush")
    {
        effectsProcessor.setBitcrush(newValue);
    }
    else if (parameterID == "width")
    {
        effectsProcessor.setStereoWidth(newValue);
    }
    else if (parameterID == "output")
    {
        outputLevel = newValue;
    }
    else if (parameterID == "dryWet")
    {
        dryWetMix = newValue;
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout NoiseLabAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    // Noise Type
    params.add(std::make_unique<juce::AudioParameterChoice>(
        "noiseType",
        "Noise Type",
        juce::StringArray({"White", "Pink", "Brown", "Digital", "Analog"}),
        0  // default to White Noise
    ));
    
    // Trigger Mode
    params.add(std::make_unique<juce::AudioParameterChoice>(
        "triggerMode",
        "Trigger Mode",
        juce::StringArray({"Free Run", "MIDI Trigger", "Host Sync", "One-Shot"}),
        1  // default to MIDI Trigger
    ));
    
    // Envelope
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "attack",
        "Attack",
        juce::NormalisableRange<float>(1.0f, 10000.0f, 0.1f, 0.3f),  // ms, logarithmic scaling
        10.0f  // default
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "decay",
        "Decay",
        juce::NormalisableRange<float>(1.0f, 30000.0f, 0.1f, 0.3f),  // ms, logarithmic scaling
        100.0f  // default
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "sustain",
        "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f  // default
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "release",
        "Release",
        juce::NormalisableRange<float>(1.0f, 30000.0f, 0.1f, 0.3f),  // ms, logarithmic scaling
        500.0f  // default
    ));
    
    // LFO
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "lfoRate",
        "LFO Rate",
        juce::NormalisableRange<float>(0.1f, 50.0f, 0.01f, 0.3f),  // Hz, logarithmic scaling
        1.0f  // default
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "lfoDepth",
        "LFO Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f  // default to OFF
    ));
    
    params.add(std::make_unique<juce::AudioParameterBool>(
        "lfoSync",
        "LFO Sync",
        false  // default
    ));
    
    params.add(std::make_unique<juce::AudioParameterChoice>(
        "lfoTarget",
        "LFO Target",
        juce::StringArray({"Volume", "Filter Cutoff", "Filter Resonance", "Pitch"}),
        0  // default to Volume
    ));
    
    // Filter
    params.add(std::make_unique<juce::AudioParameterChoice>(
        "filterType",
        "Filter Type",
        juce::StringArray({"Low Pass", "Band Pass", "High Pass"}),
        0  // default to Low Pass
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "cutoff",
        "Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f),  // Hz, logarithmic scaling
        1000.0f  // default
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "resonance",
        "Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f  // default
    ));
    
    // Effects
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "drive",
        "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f  // default
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "bitcrush",
        "Bitcrush",
        juce::NormalisableRange<float>(1.0f, 16.0f, 0.1f),
        16.0f  // default (no effect)
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "width",
        "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f),
        1.0f  // default (normal stereo)
    ));
    
    // Global
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "output",
        "Output Level",
        juce::NormalisableRange<float>(-70.0f, 6.0f, 0.1f),
        0.0f  // default (0 dB)
    ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
        "dryWet",
        "Dry/Wet",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f  // default (100% wet)
    ));
    
    return params;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NoiseLabAudioProcessor();
}