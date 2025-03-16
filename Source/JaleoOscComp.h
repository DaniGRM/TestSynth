/*
  ==============================================================================

    JaleoOsc.h
    Created: 16 Mar 2025 5:31:53pm
    Author:  danii

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GUI.h"

class JaleoOscComp : public juce::Component
{
public:
    JaleoOscComp(TestSynthAudioProcessor& p, int oscIndex)
        : audioProcessor(p),
        index(oscIndex),
        attackSlider(*p.apvts.getParameter("Attack" + juce::String(oscIndex))),
        decaySlider(*p.apvts.getParameter("Decay" + juce::String(oscIndex))),
        sustainSlider(*p.apvts.getParameter("Sustain" + juce::String(oscIndex))),
        releaseSlider(*p.apvts.getParameter("Release" + juce::String(oscIndex))),
        attackAttachment(p.apvts, "Attack" + juce::String(oscIndex), attackSlider),
        decayAttachment(p.apvts, "Decay" + juce::String(oscIndex), decaySlider),
        sustainAttachment(p.apvts, "Sustain" + juce::String(oscIndex), sustainSlider),
        releaseAttachment(p.apvts, "Release" + juce::String(oscIndex), releaseSlider),
        sineButton("sineBtn" + juce::String(oscIndex), 0),
        triangularButton("triangularBtn" + juce::String(oscIndex), 1),
        squareButton("squareBtn" + juce::String(oscIndex), 2),
        sawButton("sawBtn" + juce::String(oscIndex), 3)
    {
        // Añadir botones de selección de forma de onda
        sineButton.setToggleState(true, juce::dontSendNotification);
        triangularButton.setToggleState(false, juce::dontSendNotification);
        squareButton.setToggleState(false, juce::dontSendNotification);
        sawButton.setToggleState(false, juce::dontSendNotification);

        sineButton.onClick = [this] { selectWaveform(0); };
        triangularButton.onClick = [this] { selectWaveform(1); };
        squareButton.onClick = [this] { selectWaveform(2); };
        sawButton.onClick = [this] { selectWaveform(3); };

        for (auto* component : getComps()) {
            addAndMakeVisible(component);
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
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

    void setLabel(juce::Label& label, juce::String text, juce::Rectangle<int>bounds) {
        label.setBounds(bounds);
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centredTop);
        label.setColour(juce::Label::textColourId, juce::Colour::fromRGB(6, 35, 111));
        label.setFont(andalusian);
    }
private:

    int index;
    TestSynthAudioProcessor& audioProcessor;

    MyRotarySlider attackSlider, decaySlider, sustainSlider, releaseSlider;
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;

    WaveformButton sineButton, triangularButton, squareButton, sawButton;

    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel, title;

    std::vector<juce::Component*> getComps()
    {
        return { 
            &attackSlider,
            &decaySlider,
            &sustainSlider,
            &releaseSlider,
            &attackLabel,
            &decayLabel,
            &sustainLabel,
            &releaseLabel,
            &sineButton,
            &triangularButton,
            &squareButton,
            &sawButton
        };
    }



    void selectWaveform(int newWaveType)
    {
        auto* waveParam = audioProcessor.apvts.getParameter("WaveType" + juce::String(index));
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
};