/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestSynthAudioProcessorEditor::TestSynthAudioProcessorEditor (TestSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{    
    setSize(600, 400);
    for (auto* component : getComps()) {
        addAndMakeVisible(component);
    }
}

TestSynthAudioProcessorEditor::~TestSynthAudioProcessorEditor()
{
}

//==============================================================================
void TestSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (WHITE);
}

void TestSynthAudioProcessorEditor::resized()
{
    for (int i = 0; i < audioProcessor.numOscs; i++){
        oscillators.push_back(std::make_unique<JaleoOscComp>(audioProcessor, i));
        addAndMakeVisible(*oscillators[i]);
    }

    auto bounds = getLocalBounds();

    auto header = bounds.removeFromTop(bounds.getHeight() * 0.15f);

    title.setBounds(header);
    title.setText("Jaleo", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centredBottom);
    title.setColour(juce::Label::textColourId, juce::Colour::fromRGB(6, 35, 111));
    title.setFont(titleFont);

    if (!oscillators.empty()) {

        for (int i = 0; i < audioProcessor.numOscs; ++i) // De momento 2 osciladores, pero puedes ampliarlo
        {
            auto synthBounds = bounds.removeFromTop(bounds.getHeight() * ((float)(1) / (float)(audioProcessor.numOscs - i)));
            oscillators[i]->setBounds(synthBounds);
        }
    }
}

std::vector<juce::Component*> TestSynthAudioProcessorEditor::getComps()
{
    return
    {
        &title
    };
}
