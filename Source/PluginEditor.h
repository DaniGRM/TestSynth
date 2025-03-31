/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI.h"
#include "JaleoOscComp.h"


//==============================================================================
/**
*/
class TestSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TestSynthAudioProcessorEditor (TestSynthAudioProcessor&);
    ~TestSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TestSynthAudioProcessor& audioProcessor;
    std::vector<std::unique_ptr< JaleoOscComp>> oscillators;

    juce::Label title;

    std::vector<juce::Component*> getComps();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestSynthAudioProcessorEditor)
};
