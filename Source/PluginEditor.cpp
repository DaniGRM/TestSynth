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
    releaseSliderAttachment(p.apvts, "Release", releaseSlider),
    sineButton("sineBtn", 0),
    triangularButton("triangularBtn", 1),
    squareButton("squareBtn", 2),
    sawButton("sawBtn", 3)
{
    // Make sure that before the constructor has finished, you've set the
    
    setSize(600, 400);
    // Asigna el LookAndFeel personalizado al slider (y a otros componentes si lo deseas)
    for (auto* component : getComps()) {
        addAndMakeVisible(component);
    }

    sineButton.setToggleState(true, juce::dontSendNotification);
    triangularButton.setToggleState(false, juce::dontSendNotification);
    squareButton.setToggleState(false, juce::dontSendNotification);
    sawButton.setToggleState(false, juce::dontSendNotification);

    sineButton.onClick = [this] { selectWaveform(0); };
    triangularButton.onClick = [this] { selectWaveform(1); };
    squareButton.onClick = [this] { selectWaveform(2); };
    sawButton.onClick = [this] { selectWaveform(3); };
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

    auto bounds = getLocalBounds();

    auto header = bounds.removeFromTop(bounds.getHeight() * 0.15f);

    title.setBounds(header);
    title.setText("Jaleo", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centredBottom);
    title.setColour(juce::Label::textColourId, juce::Colour::fromRGB(6, 35, 111));
    title.setFont(titleFont);
    paintOsc(bounds);
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
        &sineButton,
        &triangularButton,
        &squareButton,
        &sawButton,
        &title
    };
}

void TestSynthAudioProcessorEditor::paintOsc(juce::Rectangle<int>bounds) 
{

    juce::Array< MyRotarySlider*>osc_sliders = { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider };
    juce::Array< juce::Label*>osc_labels = { &attackLabel, &decayLabel, &sustainLabel, &releaseLabel };
    juce::Array< juce::String>osc_labels_text = { "Attack", "Decay", "Sustain", "Release" };
    juce::Array< WaveformButton*>osc_buttons = { &sineButton, &triangularButton, &squareButton, &sawButton };
    for (int i = 0; i < 4; i++) {
        auto slider_bounds = bounds.removeFromLeft(bounds.getWidth() / (4 - i));
        auto btn_bounds = slider_bounds.removeFromTop(slider_bounds.getHeight() * 0.25);
        auto label_bounds = slider_bounds.removeFromBottom(slider_bounds.getHeight() * 0.33);
        osc_buttons[i]->setBounds(btn_bounds);
        osc_sliders[i]->setBounds(slider_bounds);
        setLabel(*osc_labels[i], osc_labels_text[i], label_bounds);
    }

}

void TestSynthAudioProcessorEditor::setLabel(juce::Label& label, juce::String text, juce::Rectangle<int>bounds) {
    label.setBounds(bounds);
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centredTop);
    label.setColour(juce::Label::textColourId, juce::Colour::fromRGB(6, 35, 111));
    label.setFont(andalusian);
    //label.setFont(TukyUI::Fonts::label);
}

void TestSynthAudioProcessorEditor::selectWaveform(int newWaveType)
{
    auto* waveParam = audioProcessor.apvts.getParameter("WaveType");
    if (waveParam != nullptr)
    {
        float normalizedValue = waveParam->convertTo0to1(static_cast<float>(newWaveType));
        waveParam->setValueNotifyingHost(normalizedValue);
    }

    sineButton.setToggleState(newWaveType == 0, juce::dontSendNotification);
    triangularButton.setToggleState(newWaveType == 1, juce::dontSendNotification);
    squareButton.setToggleState(newWaveType == 2, juce::dontSendNotification);
    sawButton.setToggleState(newWaveType == 3, juce::dontSendNotification);
}