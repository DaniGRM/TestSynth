/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestSynthAudioProcessorEditor::TestSynthAudioProcessorEditor (TestSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    attackSlider(*p.apvts.getParameter("Attack")),
    decaySlider(*p.apvts.getParameter("Decay")),
    sustainSlider(*p.apvts.getParameter("Sustain")),
    releaseSlider(*p.apvts.getParameter("Release")),
    attackSliderAttachment(p.apvts, "Attack", attackSlider),
    decaySliderAttachment(p.apvts, "Decay", decaySlider),
    sustainSliderAttachment(p.apvts, "Sustain", sustainSlider),
    releaseSliderAttachment(p.apvts, "Release", releaseSlider)
{
    // Make sure that before the constructor has finished, you've set the
    
    setSize(600, 400);
    // Asigna el LookAndFeel personalizado al slider (y a otros componentes si lo deseas)
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::whitesmoke);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void TestSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    juce::Array< MyRotarySlider*>osc_sliders = { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider };
    juce::Array< juce::Label*>osc_labels = { &attackLabel, &decayLabel, &sustainLabel, &releaseLabel };
    juce::Array< juce::String>osc_labels_text = { "ATTACK", "DECAY", "SUSTAIN", "RELEASE"};
    auto bounds = getLocalBounds();
    for (int i = 0; i < 4; i++) {
        auto first_bounds = bounds.removeFromLeft(bounds.getWidth() / (4 - i));
        first_bounds.removeFromTop(first_bounds.getHeight() * 0.25);
        auto first_label_bounds = first_bounds.removeFromBottom(first_bounds.getHeight() * 0.33);
        osc_sliders[i]->setBounds(first_bounds);
        setLabel(*osc_labels[i], osc_labels_text[i], first_label_bounds);
    }
   
    paintOsc(bounds);


    //attackLabel.setText("attack", juce::dontSendNotification);
    //attackLabel.setBounds(0, 0, 30, 100);
    //attackLabel.setJustificationType(juce::Justification::centredBottom);
    //attackSlider.setBounds(30, 0, 100, 100);

    //decayLabel.setText("decay", juce::dontSendNotification);
    //decayLabel.setBounds(0, 100, 30, 100);
    //decayLabel.setJustificationType(juce::Justification::centredBottom);
    //decaySlider.setBounds(30, 100, 100, 100);

    //sustainLabel.setText("decay", juce::dontSendNotification);
    //sustainLabel.setBounds(0, 200, 30, 100);
    //sustainLabel.setJustificationType(juce::Justification::centredBottom);
    //sustainSlider.setBounds(30, 200, 100, 100);

    //releaseLabel.setText("decay", juce::dontSendNotification);
    //releaseLabel.setBounds(0, 300, 30, 100);
    //releaseLabel.setJustificationType(juce::Justification::centredBottom);
    //releaseSlider.setBounds(30, 300, 100, 100);
}

std::vector<juce::Component*> TestSynthAudioProcessorEditor::getComps()
{
    return
    {
        &attackSlider,
        &attackLabel,
        &decaySlider,
        &decayLabel,
        &sustainSlider,
        &sustainLabel,
        &releaseSlider,
        &releaseLabel,
    };
}

void TestSynthAudioProcessorEditor::paintOsc(juce::Rectangle<int>bounds) 
{


}

void TestSynthAudioProcessorEditor::setLabel(juce::Label& label, juce::String text, juce::Rectangle<int>bounds) {
    label.setBounds(bounds);
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centredTop);
    label.setColour(juce::Label::textColourId, juce::Colour::fromRGB(6, 35, 111));
    label.setFont(TukyUI::Fonts::label);
    //label.setColour(0, );
}
