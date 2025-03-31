/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestSynthAudioProcessor::TestSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

    for (int i = 0; i < numOscs; i++) {
        osc.push_back(std::make_unique<juce::Synthesiser>());
        osc.back()->clearVoices();
        for (int i = 0; i < 6; i++) {
            osc.back()->addVoice(new JaleoVoice());
        }
        osc.back()->clearSounds();
        osc.back()->addSound(new JaleoSound());
    }

}

TestSynthAudioProcessor::~TestSynthAudioProcessor()
{
}

//==============================================================================
const juce::String TestSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TestSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TestSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TestSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TestSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TestSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TestSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TestSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TestSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void TestSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TestSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    

    for (int i = 0; i < numOscs; i++) {
        osc[i]->setCurrentPlaybackSampleRate(sampleRate);
    }
    updateOscs();
}

void TestSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TestSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TestSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    updateOscs();
    for (int i = 0; i < numOscs; i++) {
        osc[i]->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool TestSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TestSynthAudioProcessor::createEditor()
{
    return new TestSynthAudioProcessorEditor (*this);
}

//==============================================================================
void TestSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TestSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}



juce::AudioProcessorValueTreeState::ParameterLayout TestSynthAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    for (int i = 0; i < numOscs; i++) {
        layout.add(std::make_unique<juce::AudioParameterFloat>("Attack" + juce::String(i), "Attack" + juce::String(i), 0.f, 2.f, 0.1f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Decay" + juce::String(i), "Decay" + juce::String(i), 0.f, 2.f, 0.2f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain" + juce::String(i), "Sustain" + juce::String(i), 0.f, 2.f, 0.8f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Release" + juce::String(i), "Release" + juce::String(i), 0.f, 5.f, 1.5f));
        layout.add(std::make_unique<juce::AudioParameterInt>("WaveType" + juce::String(i), "WaveType" + juce::String(i), 0, 3, 0));
    }

    return layout;
}

void TestSynthAudioProcessor::updateOscs() {
    for (int i = 0; i < numOscs; i++) {
        updateADSR(i);
        updateWaveType(i);
    }
}
void TestSynthAudioProcessor::updateADSR(int oscIndex)
{
    for (int i = 0; i < osc[oscIndex]->getNumVoices(); i++)
    {
        if (auto* voice = dynamic_cast<JaleoVoice*>(osc[oscIndex]->getVoice(i)))
        {
            voice->setADSR(*apvts.getRawParameterValue("Attack" + juce::String(oscIndex)),
                *apvts.getRawParameterValue("Decay" + juce::String(oscIndex)),
                *apvts.getRawParameterValue("Sustain" + juce::String(oscIndex)),
                *apvts.getRawParameterValue("Release" + juce::String(oscIndex)));
        }
    }
}

void TestSynthAudioProcessor::updateWaveType(int oscIndex)
{
    for (int i = 0; i < osc[oscIndex]->getNumVoices(); i++)
    {    
        if (auto* voice = dynamic_cast<JaleoVoice*>(osc[oscIndex]->getVoice(i)))
        {
            voice->setWaveType(*apvts.getRawParameterValue("WaveType" + juce::String(oscIndex)));
        }
    }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TestSynthAudioProcessor();
}
