/*
  ==============================================================================

    JaleoVoice.h
    Created: 12 Feb 2025 6:53:52pm
    Author:  danii

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "JaleoSound.h"

class JaleoVoice : public juce::SynthesiserVoice
{
public:

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<JaleoSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override
    {
        adsr.reset();
        adsr.setSampleRate(getSampleRate());
        adsr.noteOn();
        phase = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        phaseDelta = (cyclesPerSecond / getSampleRate()) * juce::MathConstants<double>::twoPi;
    }

    void stopNote(float /*velocity*/, bool allowTailOff)
    {
        adsr.noteOff();

        if (!allowTailOff) {
            clearCurrentNote();
            phaseDelta = 0.0;
        }
    }

    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        // Operate only if phaseDelta has been configured
        if (phaseDelta != 0.0)
        {
            bool applyingTailOff = (tailOff > 0.0);

            while (--numSamples >= 0)
            {
                auto currentSample = (float)(getWaveValue() * level * adsr.getNextSample());

                if (applyingTailOff)
                {
                    currentSample *= tailOff;
                    tailOff *= 0.99; // Exponential decay to simulate envelope

                    if (tailOff <= 0.005) // If tailOff is under a threshold, assume note has ended
                    {
                        clearCurrentNote(); // Clear the voice -> available to be reused
                        phaseDelta = 0.0;   // Reset phaseDelta for the next voice
                        break;
                    }
                }
                // Add it to all channels
                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);
                // Increase phase and sample index to write next iteration
                phase += phaseDelta;
                if (phase >= juce::MathConstants<float>::twoPi)
                    phase -= juce::MathConstants<float>::twoPi;

                ++startSample;
            }
        }
    }

    void pitchWheelMoved(int newPitchWheelValue) override
    {

    }

    void controllerMoved(int controllerNumber, int newControllerValue) override
    {

    }

    void setADSR(float newAttack, float newDecay, float newSustain, float newRelease)
    {
        adsrParams.attack = newAttack;
        adsrParams.decay = newDecay;
        adsrParams.sustain = newSustain;
        adsrParams.release = newRelease;
        adsr.setParameters(adsrParams);
    }

    void setWaveType(int newType)
    {
        waveType = newType;
    }
private:
    // phase: phase
    // phaseDelta: phase increase
    // level: volume
    // tailOff

    double phase = 0.0, phaseDelta = 0.0, level = 0.0, tailOff = 0.0;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    int waveType = 0;

    float getWaveValue() {

        float value = 0.f;
        switch (waveType)
        {
        case 0:
            value = (float)(std::sin(phase));
            break;
        case 1:
            value = (float)(2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(phase));
            break;
        case 2:
            value = (float)(std::sin(phase) >= 0 ? 1.0f : -1.0f);
            break;
        case 3:
            value = (float)(2.0f * (phase / juce::MathConstants<float>::twoPi) - 1.0f);
            break;
        default:
            break;
        }

        return value;
    }
};