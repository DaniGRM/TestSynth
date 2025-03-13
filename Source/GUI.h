/*
  ==============================================================================

    GUI.h
    Created: 12 Mar 2025 5:58:18pm
    Author:  danii

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once
class MyLookAndFeel : public juce::LookAndFeel_V4
{
public:

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override
    {
        using namespace juce;

        auto bounds = Rectangle<float>(x, y, width, height);
        auto center = bounds.getCentre();
        auto radius = jmin(width, height) * 0.4f;  // Radio del slider

        // Dibujar fondo del slider
        g.setColour(Colours::whitesmoke);
        g.fillEllipse(bounds.reduced(width * 0.1f));

        // Dibujar borde del slider
        g.setColour(Colour::fromRGBA(6, 35, 111, 120));
        g.drawEllipse(bounds.reduced(width * 0.1f), 2.0f);

        // Obtener el ángulo del slider basado en su posición
        float sliderAngle = jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);

        // Dibujar estrella de 8 puntas

        float innerRadius = radius * 0.61f;
        float outerRadius = radius * 0.8;

        Path starPath;
        int numPoints = 8;
        float angleStep = MathConstants<float>::pi / numPoints;

        // Índice de la punta fija que actuará como indicador
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

        // Aplicar rotación al path completo, incluyendo la punta indicadora
        AffineTransform rotation = AffineTransform().rotated(sliderAngle, center.getX(), center.getY());
        starPath.applyTransform(rotation);

        // Dibujar la estrella base
        g.setColour(Colour::fromRGB(6, 35, 111));
        g.fillPath(starPath);

        // Pintar la punta indicadora con un color diferente
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

        // Aplicar la misma rotación a la punta indicadora
        indicatorPath.applyTransform(rotation);

        // Dibujar la punta indicadora
        g.setColour(Colour::fromRGB(180, 62, 35));
        g.fillPath(indicatorPath);
    }
};

struct MyRotarySlider : juce::Slider
{
    MyRotarySlider(juce::RangedAudioParameter& rap) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap)
    {
        setLookAndFeel(&lnf);
    }

    ~MyRotarySlider() {
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
            jmap(normalizedValue, range.getStart(), range.getEnd(), 0.0, 1.0),
            startAng,
            endAng,
            *this);
    }

    juce::Rectangle<int> getSliderBounds() const {

        auto bounds = getLocalBounds();

        // Get original bounds
        auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

        // Substract for text height
        size -= 0.f;

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
