#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

// --- Reverser Card UI ---
class ReverserCard : public juce::Component
{
public:
    ReverserCard(juce::AudioProcessorValueTreeState& apvts)
    {
        mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        mixSlider.setLookAndFeel(&purpleStyle);
        addAndMakeVisible(mixSlider);

        timeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        timeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        timeSlider.setLookAndFeel(&purpleStyle);
        addAndMakeVisible(timeSlider);

        mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "REV_MIX", mixSlider);
        timeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "REV_TIME", timeSlider);
    }

    ~ReverserCard() override
    {
        mixSlider.setLookAndFeel(nullptr);
        timeSlider.setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF121212));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText("Reverser", 15, 12, 120, 24, juce::Justification::left);

        g.setColour(juce::Colours::grey);
        g.setFont(juce::FontOptions(13.0f));
        g.drawText("Mix", mixSlider.getBounds().translated(0, mixSlider.getHeight() / 2 + 10), juce::Justification::centred);
        g.drawText("Time beat", timeSlider.getBounds().translated(0, timeSlider.getHeight() / 2 + 8), juce::Justification::centred);
    }

    void resized() override
    {
        auto area = getLocalBounds().removeFromBottom(getHeight() - 40);
        mixSlider.setBounds(area.removeFromTop(130).withSizeKeepingCentre(95, 95));
        timeSlider.setBounds(area.removeFromTop(80).withSizeKeepingCentre(48, 48));
    }

private:
    PurpleArcKnobLookAndFeel purpleStyle;
    juce::Slider mixSlider, timeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach, timeAttach;
};

// --- Noise Gate Card UI ---
class NoiseGateCard : public juce::Component
{
public:
    NoiseGateCard(juce::AudioProcessorValueTreeState& apvts)
    {
        attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(attackSlider);

        releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(releaseSlider);

        threshSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        threshSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(threshSlider);

        attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_ATTACK", attackSlider);
        releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_RELEASE", releaseSlider);
        threshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_THRESH", threshSlider);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF0F0F0F));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText("Noise Gate", 15, 12, 120, 24, juce::Justification::left);

        auto bounds = getLocalBounds();
        if (bounds.getHeight() < 180) return;

        auto scopeArea = bounds.removeFromTop(180).removeFromBottom(130).toFloat();
        if (scopeArea.getHeight() <= 0.0f) return;

        g.setColour(juce::Colours::white.withAlpha(0.05f));
        g.drawRect(scopeArea);

        juce::Path wave;
        wave.startNewSubPath(scopeArea.getX(), scopeArea.getCentreY());
        for (float x = scopeArea.getX(); x < scopeArea.getRight(); x += 5.0f)
        {
            float y = scopeArea.getCentreY() + std::sin(x * 0.05f) * 35.0f;
            wave.lineTo(x, y);
        }
        g.setColour(juce::Colour(0xFFFFB703));
        g.strokePath(wave, juce::PathStrokeType(1.5f));

        float threshVal = static_cast<float>(threshSlider.getValue());
        float normalizedThresh = juce::jmap(threshVal, -60.0f, 0.0f, scopeArea.getBottom(), scopeArea.getY());
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.drawDashedLine(juce::Line<float>(scopeArea.getX(), normalizedThresh, scopeArea.getRight(), normalizedThresh), nullptr, 0, 2);

        thresholdPillBounds = juce::Rectangle<float>(scopeArea.getCentreX() - 55.0f, normalizedThresh - 14.0f, 110.0f, 28.0f);
        g.setColour(juce::Colour(0xFF1E1E1E));
        g.fillRoundedRectangle(thresholdPillBounds, 14.0f);
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(thresholdPillBounds, 14.0f, 1.2f);
        g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
        g.drawText("Threshold  ↕", thresholdPillBounds, juce::Justification::centred);

        g.drawText(juce::String(threshVal, 1) + " dB", scopeArea.removeFromLeft(80).removeFromTop(30), juce::Justification::centred);

        g.setFont(juce::FontOptions(12.0f));
        g.drawText("Attack: " + juce::String(static_cast<float>(attackSlider.getValue()), 0) + " ms", 15, getHeight() - 40, 100, 20, juce::Justification::left);
        g.drawText("Release: " + juce::String(static_cast<float>(releaseSlider.getValue()), 0) + " ms", 130, getHeight() - 40, 100, 20, juce::Justification::left);
    }

    void resized() override
    {
        attackSlider.setBounds(15, getHeight() - 25, 100, 15);
        releaseSlider.setBounds(130, getHeight() - 25, 100, 15);
        threshSlider.setBounds(0, 0, 0, 0);
    }

    void mouseDown(const juce::MouseEvent& e) override { if (thresholdPillBounds.contains(e.position)) isDraggingPill = true; }
    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (isDraggingPill)
        {
            auto bounds = getLocalBounds();
            if (bounds.getHeight() < 180) return;
            auto scopeArea = bounds.removeFromTop(180).removeFromBottom(130).toFloat();
            if (scopeArea.getHeight() <= 0.0f) return;

            float norm = juce::jlimit(0.0f, 1.0f, (e.position.y - scopeArea.getY()) / scopeArea.getHeight());
            threshSlider.setValue(juce::jmap(norm, 1.0f, 0.0f, -60.0f, 0.0f));
            repaint();
        }
    }

private:
    juce::Slider attackSlider, releaseSlider, threshSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach, releaseAttach, threshAttach;
    juce::Rectangle<float> thresholdPillBounds;
    bool isDraggingPill = false;
};

// --- Chorus Ensemble Card UI ---
class ChorusCard : public juce::Component
{
public:
    ChorusCard(juce::AudioProcessorValueTreeState& apvts)
    {
        auto setupKnob = [this](juce::Slider& slider) {
            slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            slider.setLookAndFeel(&teardropStyle);
            addAndMakeVisible(slider);
        };

        setupKnob(depthSlider); setupKnob(amountSlider); setupKnob(speedSlider); setupKnob(bassSlider); setupKnob(trebleSlider);
        depthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_DEPTH", depthSlider);
        amountAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_AMOUNT", amountSlider);
        speedAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_SPEED", speedSlider);
        bassAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_BASS", bassSlider);
        trebleAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_TREBLE", trebleSlider);
    }

    ~ChorusCard() override
    {
        depthSlider.setLookAndFeel(nullptr); amountSlider.setLookAndFeel(nullptr);
        speedSlider.setLookAndFeel(nullptr); bassSlider.setLookAndFeel(nullptr); trebleSlider.setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF6B93AC));
        SeigaihaDrawer::drawPattern(g, getLocalBounds().toFloat(), juce::Colour(0xFF3B627C));

        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText("Chorus Ensemble", 15, 12, 160, 24, juce::Justification::left);

        g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        g.drawText("Depth", depthSlider.getBounds().translated(0, depthSlider.getHeight() / 2 + 5), juce::Justification::centred);
        g.drawText("Amount", amountSlider.getBounds().translated(0, amountSlider.getHeight() / 2 + 5), juce::Justification::centred);
        g.drawText("Speed", speedSlider.getBounds().translated(0, speedSlider.getHeight() / 2 + 5), juce::Justification::centred);
        g.drawText("Bass", bassSlider.getBounds().translated(0, bassSlider.getHeight() / 2 + 5), juce::Justification::centred);
        g.drawText("Treble", trebleSlider.getBounds().translated(0, trebleSlider.getHeight() / 2 + 5), juce::Justification::centred);
    }

    void resized() override
    {
        auto topRow = getLocalBounds().removeFromTop(150).removeFromBottom(85);
        depthSlider.setBounds(topRow.removeFromLeft(getWidth() / 2).withSizeKeepingCentre(52, 52));
        amountSlider.setBounds(topRow.withSizeKeepingCentre(52, 52));

        auto bottomRow = getLocalBounds().removeFromBottom(110).removeFromTop(80);
        int third = getWidth() / 3;
        speedSlider.setBounds(bottomRow.removeFromLeft(third).withSizeKeepingCentre(42, 42));
        bassSlider.setBounds(bottomRow.removeFromLeft(third).withSizeKeepingCentre(42, 42));
        trebleSlider.setBounds(bottomRow.withSizeKeepingCentre(42, 42));
    }

private:
    TeardropKnobLookAndFeel teardropStyle;
    juce::Slider depthSlider, amountSlider, speedSlider, bassSlider, trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach, amountAttach, speedAttach, bassAttach, trebleAttach;
};

// --- Editor Core ---
class ModularFXAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ModularFXAudioProcessorEditor(ModularFXAudioProcessor& p);
    ~ModularFXAudioProcessorEditor() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ModularFXAudioProcessor& audioProcessor;

    ReverserCard reverserCard;
    NoiseGateCard noiseGateCard;
    ChorusCard chorusCard;
};
