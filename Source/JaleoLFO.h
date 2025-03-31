/*
  ==============================================================================

    JaleoLFO.h
    Created: 31 Mar 2025 5:20:42pm
    Author:  danii

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class JaleoLFO {
public:
    void setFrequency(float f) {
        freq = f;
        updateAngle();
    }

    void setSampleRate(float fs) {
        sampleRate = fs;
    }

    float getNextSample() {
        float sample = std::cos(currentAngle);
        currentAngle += angleDelta;
        if (currentAngle > juce::MathConstants<float>::twoPi)
            currentAngle -= juce::MathConstants<float>::twoPi;

        return sample;
    }
    float getNextAngle() {
        float sample = currentAngle;
        currentAngle += angleDelta;
        if (currentAngle > juce::MathConstants<float>::twoPi)
            currentAngle -= juce::MathConstants<float>::twoPi;

        return sample;
    }

private:
    float sampleRate = 44100.f, freq = 1.f, currentAngle = 0.f, angleDelta = (juce::MathConstants<float>::twoPi * 1.f / 44100.f);

    void updateAngle() {
        angleDelta = (juce::MathConstants<float>::twoPi * freq / sampleRate);
    }
};