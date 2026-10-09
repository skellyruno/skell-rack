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
        g.drawText("Mix", mixSlider.getX(), mixSlider.getBottom() + 2, mixSlider.getWidth(), 18, juce::Justification::centred);
        g.drawText("Time", timeSlider.getX(), timeSlider.getBottom() + 2, timeSlider.getWidth(), 18, juce::Justification::centred);
    }

    void resized() override
    {
        mixSlider.setBounds(getWidth() / 2 - 40, 50, 80, 80);
        timeSlider.setBounds(getWidth() / 2 - 25, 170, 50, 50);
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
        attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        addAndMakeVisible(attackSlider);

        releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        addAndMakeVisible(releaseSlider);

        threshSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        threshSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
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

        g.setFont(juce::FontOptions(12.0f));
        g.setColour(juce::Colours::grey);
        g.drawText("Threshold", threshSlider.getX(), threshSlider.getY() - 16, threshSlider.getWidth(), 16, juce::Justification::centred);
        g.drawText("Attack", attackSlider.getX(), attackSlider.getY() - 16, attackSlider.getWidth(), 16, juce::Justification::centred);
        g.drawText("Release", releaseSlider.getX(), releaseSlider.getY() - 16, releaseSlider.getWidth(), 16, juce::Justification::centred);
    }

    void resized() override
    {
        int w = getWidth() - 30;
        threshSlider.setBounds(15, 60, w, 45);
        attackSlider.setBounds(15, 130, w, 45);
        releaseSlider.setBounds(15, 200, w, 45);
    }

private:
    juce::Slider attackSlider, releaseSlider, threshSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach, releaseAttach, threshAttach;
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
    }

    void resized() override
    {
        int third = getWidth() / 3;
        depthSlider.setBounds(20, 50, 55, 55);
        amountSlider.setBounds(getWidth() - 75, 50, 55, 55);

        speedSlider.setBounds(5, 150, third - 10, 50);
        bassSlider.setBounds(third + 5, 150, third - 10, 50);
        trebleSlider.setBounds(third * 2 + 5, 150, third - 10, 50);
    }

private:
    TeardropKnobLookAndFeel teardropStyle;
    juce::Slider depthSlider, amountSlider, speedSlider, bassSlider, trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach, amountAttach, speedAttach, bassAttach, trebleAttach;
};

// --- Main Editor ---
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
