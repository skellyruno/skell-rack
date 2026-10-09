#pragma once
#include <JuceHeader.h>

class SeigaihaDrawer
{
public:
    static void drawPattern(juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour waveColor)
    {
        if (bounds.isEmpty()) return;

        g.setColour(waveColor.withAlpha(0.25f));
        const float radiusStep = 14.0f;
        const float rowSpacing = 22.0f;
        const float colSpacing = 44.0f;

        for (float y = bounds.getBottom() + 20.0f; y > bounds.getY() + 80.0f; y -= rowSpacing)
        {
            const float rowDiff = (bounds.getBottom() - y) / rowSpacing;
            const float xOffset = (static_cast<int>(rowDiff) % 2 == 0) ? 0.0f : colSpacing * 0.5f;
            for (float x = bounds.getX() - 20.0f + xOffset; x < bounds.getRight() + 20.0f; x += colSpacing)
            {
                for (int r = 4; r >= 1; --r)
                {
                    const float currentRadius = static_cast<float>(r) * radiusStep;
                    juce::Path arc;
                    arc.addCentredArc(x, y, currentRadius, currentRadius, 0.0f,
                                      -juce::MathConstants<float>::halfPi,
                                      juce::MathConstants<float>::halfPi, true);
                    g.strokePath(arc, juce::PathStrokeType(1.5f));
                }
            }
        }
    }
};

class TeardropKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& /*slider*/) override
    {
        if (width <= 0 || height <= 0) return;

        auto bounds = juce::Rectangle<float>(static_cast<float>(x),
                                            static_cast<float>(y),
                                            static_cast<float>(width),
                                            static_cast<float>(height)).reduced(6.0f);
        if (bounds.isEmpty()) return;

        auto center = bounds.getCentre();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        if (radius <= 2.0f) return;

        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        g.setColour(juce::Colours::black.withAlpha(0.15f));
        g.fillEllipse(bounds.translated(0.0f, 3.0f));

        juce::Path teardrop;
        float topY = center.y - radius;
        float bulbRadius = radius * 0.65f;

        teardrop.startNewSubPath(center.x, topY);
        teardrop.cubicTo(center.x + radius, center.y - bulbRadius,
                         center.x + bulbRadius, center.y + radius,
                         center.x, center.y + radius);
        teardrop.cubicTo(center.x - bulbRadius, center.y + radius,
                         center.x - radius, center.y - bulbRadius,
                         center.x, topY);
        teardrop.closeSubPath();

        teardrop.applyTransform(juce::AffineTransform::rotation(angle, center.x, center.y));

        g.setColour(juce::Colours::white);
        g.fillPath(teardrop);

        g.setColour(juce::Colour(0xFF7A9BB0).withMultipliedAlpha(0.6f));
        g.strokePath(teardrop, juce::PathStrokeType(1.2f));
    }
};

class PurpleArcKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& /*slider*/) override
    {
        if (width <= 0 || height <= 0) return;

        auto bounds = juce::Rectangle<float>(static_cast<float>(x),
                                            static_cast<float>(y),
                                            static_cast<float>(width),
                                            static_cast<float>(height)).reduced(8.0f);
        if (bounds.isEmpty()) return;

        auto center = bounds.getCentre();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        if (radius <= 2.0f) return;

        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        juce::Path arcPath;
        arcPath.addCentredArc(center.x, center.y, radius + 4.0f, radius + 4.0f, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(juce::Colour(0xFFA020F0));
        g.strokePath(arcPath, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path trackPath;
        trackPath.addCentredArc(center.x, center.y, radius + 4.0f, radius + 4.0f, 0.0f, angle, rotaryEndAngle, true);
        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.strokePath(trackPath, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour(juce::Colours::white);
        g.fillEllipse(bounds);

        juce::Path pointer;
        pointer.startNewSubPath(center);
        pointer.lineTo(center.x + std::sin(angle) * (radius * 0.7f),
                       center.y - std::cos(angle) * (radius * 0.7f));
        g.setColour(juce::Colour(0xFF1A1A1A));
        g.strokePath(pointer, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
};
