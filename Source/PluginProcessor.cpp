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

    for (int i = 0; i < numSynths; i++) {
        osc.push_back(std::make_unique<JaleoOsc>());
        osc.back()->synth.clearVoices();
        for (int i = 0; i < 6; i++) {
            osc.back()->synth.addVoice(new MyVoice());
        }
        osc.back()->synth.clearSounds();
        osc.back()->synth.addSound(new MySound());
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

    

    for (int i = 0; i < numSynths; i++) {
        osc[i]->synth.setCurrentPlaybackSampleRate(sampleRate);
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
    for (int i = 0; i < numSynths; i++) {
        osc[i]->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
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
    for (int i = 0; i < numSynths; i++) {
        layout.add(std::make_unique<juce::AudioParameterFloat>("Attack" + juce::String(i), "Attack" + juce::String(i), 0.f, 2.f, 0.1f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Decay" + juce::String(i), "Decay" + juce::String(i), 0.f, 2.f, 0.2f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain" + juce::String(i), "Sustain" + juce::String(i), 0.f, 2.f, 0.8f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Release" + juce::String(i), "Release" + juce::String(i), 0.f, 2.f, 1.5f));
        layout.add(std::make_unique<juce::AudioParameterInt>("WaveType" + juce::String(i), "WaveType" + juce::String(i), 0, 3, 0));
    }

    return layout;
}

void TestSynthAudioProcessor::updateOscs() {
    for (int i = 0; i < numSynths; i++) {
        updateADSR(i);
        updateWaveType(i);
    }
}
void TestSynthAudioProcessor::updateADSR(int oscIndex)
{
    osc[oscIndex]->attack = *apvts.getRawParameterValue("Attack" + juce::String(oscIndex));
    osc[oscIndex]->decay = *apvts.getRawParameterValue("Decay" + juce::String(oscIndex));
    osc[oscIndex]->sustain = *apvts.getRawParameterValue("Sustain" + juce::String(oscIndex));
    osc[oscIndex]->release = *apvts.getRawParameterValue("Release" + juce::String(oscIndex));

    for (int i = 0; i < osc[oscIndex]->synth.getNumVoices(); i++)
    {

        if (auto* voice = dynamic_cast<MyVoice*>(osc[oscIndex]->synth.getVoice(i)))
        {
            voice->setADSR(osc[oscIndex]->attack, osc[oscIndex]->decay, osc[oscIndex]->sustain, osc[oscIndex]->release);
        }
    }
}

void TestSynthAudioProcessor::updateWaveType(int oscIndex)
{
    osc[oscIndex]->waveType = *apvts.getRawParameterValue("WaveType" + juce::String(oscIndex));
    for (int i = 0; i < osc[oscIndex]->synth.getNumVoices(); i++)
    {    

        if (auto* voice = dynamic_cast<MyVoice*>(osc[oscIndex]->synth.getVoice(i)))
        {
            voice->setWaveType(osc[oscIndex]->waveType);
        }
    }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TestSynthAudioProcessor();
}
