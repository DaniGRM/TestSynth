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
    // Make sure that before the constructor has finished, you've set the
    
    setSize(600, 400);
    for (auto* component : getComps()) {
        addAndMakeVisible(component);
    }
    // Asigna el LookAndFeel personalizado al slider (y a otros componentes si lo deseas)
    //for (int i = 0; i < 1; ++i) // De momento 2 osciladores, pero puedes ampliarlo
    //{
    //    oscillators.push_back(std::make_unique<JaleoOscComp>(p, i));
    //    DBG(oscillators.size());
    //    addAndMakeVisible(*oscillators.back());

    //}
}

TestSynthAudioProcessorEditor::~TestSynthAudioProcessorEditor()
{
}

//==============================================================================
void TestSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::whitesmoke);
}

void TestSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    for (int i = 0; i < audioProcessor.numSynths; i++){
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

        for (int i = 0; i < audioProcessor.numSynths; ++i) // De momento 2 osciladores, pero puedes ampliarlo
        {
            auto synthBounds = bounds.removeFromTop(bounds.getHeight() * ((float)(i + 1) / (float)audioProcessor.numSynths));
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
