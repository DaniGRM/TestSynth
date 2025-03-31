/*
  ==============================================================================

    JaleoSound.h
    Created: 12 Feb 2025 7:12:53pm
    Author:  danii

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class JaleoSound : public juce::SynthesiserSound
{
    bool appliesToNote(int midiNoteNumber) override
    {
        return true;
    }

    bool appliesToChannel(int midiNoteNumber) override
    {
        return true;
    }
};