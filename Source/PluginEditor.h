/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TukyUI.h"
#include "GUI.h"


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

    MyRotarySlider attackSlider, decaySlider, sustainSlider, releaseSlider;
    //TukyUI::Components::TukyRotarySlider attackSlider;


    // Usings to make code more readable
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    // Slider Attachment for sliders
    Attachment attackSliderAttachment, decaySliderAttachment, sustainSliderAttachment, releaseSliderAttachment;

    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;

    std::vector<juce::Component*> getComps();
    void paintOsc(juce::Rectangle<int>bounds);
    void setLabel(juce::Label& label, juce::String text, juce::Rectangle<int>bounds);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestSynthAudioProcessorEditor)
};
