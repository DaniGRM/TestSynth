/*
  ==============================================================================

    GUI.h
    Created: 12 Mar 2025 5:58:18pm
    Author:  danii

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once


// COLORS
const juce::Colour BLUE = juce::Colour::fromRGB(6, 35, 111);
const juce::Colour ORANGE = juce::Colour::fromRGB(224, 130, 0);
const juce::Colour WHITE = juce::Colours::whitesmoke;

// FONTS
const juce::Font titleFont = juce::Font("Parisienne", 50.0f, juce::Font::bold);
const juce::Font andalusian = juce::Font("Parisienne", 25.0f, juce::Font::bold);

// LOOKANDFEEL
// With functions to draw components
class MyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // Function for rotary sliders
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override
    {
        using namespace juce;

        auto bounds = Rectangle<float>(x, y, width, height);
        auto center = bounds.getCentre();
        auto radius = jmin(width, height) * 0.4f;  // Radio del slider

        // Draw slider background
        g.setColour(WHITE);
        g.fillEllipse(bounds.reduced(width * 0.1f));

        // Draw slider border
        g.setColour(BLUE);
        g.drawEllipse(bounds.reduced(width * 0.1f), 2.0f);

        // Get slider angle in order to its position
        float sliderAngle = jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);

        // Draw 8 pointed star
        draw8pointedStar(g, radius, center, sliderAngle);
    }

    // Function to draw 8 pointed star
    void draw8pointedStar(juce::Graphics& g, int radius, juce::Point<float> center, float sliderAngle) {
        using namespace juce;
        float innerRadius = radius * 0.61f;
        float outerRadius = radius * 0.8;

        Path starPath;
        int numPoints = 8;
        float angleStep = MathConstants<float>::pi / numPoints;

        // Index for indicator point
        int indicatorIndex = 12;

        starPath.startNewSubPath(center.getX() + outerRadius * std::cos(0),
            center.getY() + outerRadius * std::sin(0));

        for (int i = 0; i <= numPoints * 2; ++i)
        {
            float angle = i * angleStep + MathConstants<float>::pi;
            float r = (i % 2 == 0) ? outerRadius : innerRadius;
            float x = center.getX() + r * std::cos(angle);
            float y = center.getY() + r * std::sin(angle);

            starPath.lineTo(x, y);
        }
        starPath.closeSubPath();

        
        AffineTransform rotation = AffineTransform().rotated(sliderAngle, center.getX(), center.getY());
        starPath.applyTransform(rotation);

        // Draw base star
        g.setColour(BLUE);
        g.fillPath(starPath);

        // Paint indicator
        float indicatorAngle = indicatorIndex * angleStep;
        Path indicatorPath;
        indicatorPath.startNewSubPath(center);
        indicatorPath.lineTo(center.getX() + innerRadius * std::cos(indicatorAngle - angleStep),
            center.getY() + innerRadius * std::sin(indicatorAngle - angleStep));
        indicatorPath.lineTo(center.getX() + outerRadius * std::cos(indicatorAngle),
            center.getY() + outerRadius * std::sin(indicatorAngle));
        indicatorPath.lineTo(center.getX() + innerRadius * std::cos(indicatorAngle + angleStep),
            center.getY() + innerRadius * std::sin(indicatorAngle + angleStep));
        indicatorPath.closeSubPath();

        // Apply same rotation to indicator
        indicatorPath.applyTransform(rotation);

        // Draw indicator
        g.setColour(ORANGE);
        g.fillPath(indicatorPath);
    }
};

// 
struct Star8Slider : juce::Slider
{
    Star8Slider(juce::RangedAudioParameter& rap) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap)
    {
        setLookAndFeel(&lnf);
    }

    ~Star8Slider() {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override{
        using namespace juce;

        // Set start and end angle
        auto startAng = degreesToRadians(180.f + 45.f);
        auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

        // Get the range (Slider function) and bounds (Own function)
        auto range = getRange();
        auto sliderBounds = getSliderBounds();

        double normalizedValue = (getValue() - range.getStart()) / (range.getEnd() - range.getStart());
        // Draw the slider
        getLookAndFeel().drawRotarySlider(g,
            sliderBounds.getX(),
            sliderBounds.getY(),
            sliderBounds.getWidth(),
            sliderBounds.getHeight(),
            jmap(normalizedValue, range.getStart(), range.getEnd(), 0.0, static_cast<double>(param->getNormalisableRange().end)),
            startAng,
            endAng,
            *this);
    }

    juce::Rectangle<int> getSliderBounds() const {

        auto bounds = getLocalBounds();

        // Get original bounds
        auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

        // Create rectangle in order to be the bounds itself
        juce::Rectangle<int> r;
        r.setSize(size, size);
        r.setCentre(bounds.getCentreX(), 0);
        r.setY(2);

        return r;
    }

private:

    // Slider own LookAndFeel
    MyLookAndFeel lnf;

    // Param itself to attach component
    juce::RangedAudioParameter* param;
};

class WaveformButton : public juce::Button
{
public:
    WaveformButton(const juce::String& name, int wave_type)
        : juce::Button(name)
    {
        setClickingTogglesState(true);
        type = wave_type;
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        using namespace juce;

        auto bounds = getBtnBounds().toFloat();
        
        if (getToggleState()) {
            g.setColour(BLUE);  // Active color
        }
        else {
            g.setColour(WHITE);  // Inactive color
        }
        auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
        g.fillRoundedRectangle(bounds, std::sqrt(size));
        g.setColour(BLUE);
        g.drawRoundedRectangle(bounds, std::sqrt(size), 2.0f);

        // Draw wave
        drawWave(g, bounds);
    }

private:

    void drawWave(juce::Graphics& g, juce::Rectangle<float> area)
    {
        using namespace juce;

        area.removeFromLeft(area.getWidth() * 0.2);
        area.removeFromRight(area.getWidth() * 0.25);
        Path wavePath;
        float amplitude = area.getHeight() * 0.3f;
        float midY = area.getCentreY();
        int numPoints = 100;

        for (int i = 0; i < numPoints; ++i)
        {
            float x = jmap(float(i), 0.0f, float(numPoints - 1), area.getX(), area.getRight());
            float y = 0;
            float phase = jmap(float(i), 0.0f, float(numPoints - 1), 0.f, 4.f);
            switch (type)
            {
            case 0:
                y = midY + amplitude * std::sin(jmap(float(i), 0.0f, float(numPoints - 1), 0.0f, MathConstants<float>::twoPi));
                break;
            case 1:
                y = midY + amplitude * (abs(fmod(phase - 1 + 4, 4) - 2) - 1);
                break;
            case 2:
                y = midY + amplitude * ((phase < 2.f) ? -0.8f : 0.8f);
                break;
            case 3:
                y = midY + amplitude * (fmod(phase + 2, 4) - 2) / 2;
            }
            

            if (i == 0) {
                if (type == 2) {
                    wavePath.startNewSubPath(x, midY + amplitude * 0.8f);
                    wavePath.lineTo(x, y);
                }
                else {
                    wavePath.startNewSubPath(x, y);
                }

            }
            else {
                wavePath.lineTo(x, y);
            }

            if (i == numPoints - 1 && type == 2) {
                wavePath.lineTo(x, midY -amplitude * 0.8f);
            }
                
        }

        g.setColour(getToggleState() ? WHITE : BLUE);
        g.strokePath(wavePath, PathStrokeType(2.0f));
    }

    juce::Rectangle<int> getBtnBounds() const {

        auto bounds = getLocalBounds();

        // Get original bounds
        auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

        // Apply some padding
        size -= size * 0.7f * std::exp(-size * 0.005f);

        // Create rectangle in order to be the bounds itself
        juce::Rectangle<int> r;
        r.setSize(size *1.8, size);
        r.setCentre(bounds.getCentreX(), 0);
        r.setY(bounds.getCentreY());

        return r;
    }

    MyLookAndFeel lnf;
    int type;
};
