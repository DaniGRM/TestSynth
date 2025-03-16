/*
  ==============================================================================

    MyVoice.h
    Created: 12 Feb 2025 6:53:52pm
    Author:  danii

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MySound.h"

class MyVoice : public juce::SynthesiserVoice
{
public:

    MyVoice(){

    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<MySound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override
    {
        adsr.reset();
        adsr.setSampleRate(getSampleRate());
        adsr.noteOn();
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

    void stopNote(float /*velocity*/, bool allowTailOff)
    {
        adsr.noteOff();
        if (allowTailOff) {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {

        // Operate only if angleDelta has been configured
        if (angleDelta != 0.0)
        {

            if (tailOff > 0.0) // if tailOff is != 0.0 note has been stopped
            {
                // iterate through all samples until end
                while (--numSamples >= 0)
                {
                    auto currentSample = (float)(getWaveValue() * level * tailOff * adsr.getNextSample());

                    // add it to all channels
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    // increase currentAngle and sample idx to write next iteration
                    currentAngle += angleDelta;
                    if (currentAngle >= juce::MathConstants<float>::twoPi)
                        currentAngle -= juce::MathConstants<float>::twoPi;
                    ++startSample;

                    tailOff *= 0.99; // exponential decay to simulate envelope

                    // if tailOff is under a given threshold we asume note has end
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote(); // clear the voice -> available to be reused

                        angleDelta = 0.0; // reset angleDelta for the next voice
                        break;
                    }
                }
            }
            else // if tailOff is 0.0 note is hold
            {
                while (--numSamples >= 0) // iterate until samples end
                {
                    // currentSample is: sin of currentAngle * level (volume)

                    auto currentSample = (float)(getWaveValue() * level * adsr.getNextSample());
                   
                    // add it to all channels
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                    // increase currentAngle and sample idx to write next iteration
                    currentAngle += angleDelta;
                    if (currentAngle >= juce::MathConstants<float>::twoPi)
                        currentAngle -= juce::MathConstants<float>::twoPi;

                    ++startSample;
                }
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
    // currentAngle: phase
    // angleDelta: phase increase
    // level: volume
    // tailOff

    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    int waveType = 0;

    float getWaveValue() {

        float value = 0.f;
        switch (waveType)
        {
        case 0:
            value = (float)(std::sin(currentAngle));
            break;
        case 1:
            value = (float)(2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(currentAngle));
            break;
        case 2:
            value = (float)(std::sin(currentAngle) >= 0 ? 1.0f : -1.0f);
            break;
        case 3:
            value = (float)(2.0f * (currentAngle / juce::MathConstants<float>::twoPi) - 1.0f);
            break;
        default:
            break;
        }
        return value;
    }
};