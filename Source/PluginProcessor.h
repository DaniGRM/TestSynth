/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "JaleoSound.h"
#include "JaleoVoice.h"
//==============================================================================
/**
*/

class TestSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TestSynthAudioProcessor();
    ~TestSynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Function to get parameter layout
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Parameters value tree
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    // Functions to update oscillators values from ui
    void updateADSR(int oscIndex);
    void updateWaveType(int oscIndex);
    void updateOscs();

    // Num of oscillators 
    const static int numOscs = 2;

private:
    //==============================================================================
    std::vector<std::unique_ptr< juce::Synthesiser>> osc;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestSynthAudioProcessor)
};
