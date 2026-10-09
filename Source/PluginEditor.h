#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

// --- Reverser Card UI ---
class ReverserCard : public juce::Component
{
public:
    ReverserCard(juce::AudioProcessorValueTreeState& apvts)
    {
        mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        addAndMakeVisible(mixSlider);

        timeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        timeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        addAndMakeVisible(timeSlider);

        mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "REV_MIX", mixSlider);
        timeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "REV_TIME", timeSlider);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF121212));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText("Reverser", 15, 10, 120, 24, juce::Justification::left);

        g.setFont(juce::FontOptions(12.0f));
        g.setColour(juce::Colours::grey);
        g.drawText("Mix", mixSlider.getX(), mixSlider.getY() - 18, mixSlider.getWidth(), 18, juce::Justification::centred);
        g.drawText("Time", timeSlider.getX(), timeSlider.getY() - 18, timeSlider.getWidth(), 18, juce::Justification::centred);
    }

    void resized() override
    {
        mixSlider.setBounds(getWidth() / 2 - 40, 50, 80, 100);
        timeSlider.setBounds(getWidth() / 2 - 30, 170, 60, 80);
    }

private:
    juce::Slider mixSlider, timeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach, timeAttach;
};

// --- Noise Gate Card UI ---
class NoiseGateCard : public juce::Component
{
public:
    NoiseGateCard(juce::AudioProcessorValueTreeState& apvts)
    {
        threshSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        threshSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        addAndMakeVisible(threshSlider);

        attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        addAndMakeVisible(attackSlider);

        releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        addAndMakeVisible(releaseSlider);

        threshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_THRESH", threshSlider);
        attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_ATTACK", attackSlider);
        releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_RELEASE", releaseSlider);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF0F0F0F));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText("Noise Gate", 15, 10, 120, 24, juce::Justification::left);

        g.setFont(juce::FontOptions(12.0f));
        g.setColour(juce::Colours::grey);
        g.drawText("Threshold", threshSlider.getX(), threshSlider.getY() - 18, threshSlider.getWidth(), 18, juce::Justification::centred);
        g.drawText("Attack", attackSlider.getX(), attackSlider.getY() - 18, attackSlider.getWidth(), 18, juce::Justification::centred);
        g.drawText("Release", releaseSlider.getX(), releaseSlider.getY() - 18, releaseSlider.getWidth(), 18, juce::Justification::centred);
    }

    void resized() override
    {
        int w = getWidth() - 30;
        threshSlider.setBounds(15, 60, w, 50);
        attackSlider.setBounds(15, 130, w, 50);
        releaseSlider.setBounds(15, 200, w, 50);
    }

private:
    juce::Slider threshSlider, attackSlider, releaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threshAttach, attackAttach, releaseAttach;
};

// --- Chorus Card UI ---
class ChorusCard : public juce::Component
{
public:
    ChorusCard(juce::AudioProcessorValueTreeState& apvts)
    {
        auto setupKnob = [this](juce::Slider& slider) {
            slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 16);
            addAndMakeVisible(slider);
        };

        setupKnob(depthSlider);
        setupKnob(amountSlider);
        setupKnob(speedSlider);
        setupKnob(bassSlider);
        setupKnob(trebleSlider);

        depthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_DEPTH", depthSlider);
        amountAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_AMOUNT", amountSlider);
        speedAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_SPEED", speedSlider);
        bassAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_BASS", bassSlider);
        trebleAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_TREBLE", trebleSlider);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF3B627C));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText("Chorus Ensemble", 15, 10, 160, 24, juce::Justification::left);
    }

    void resized() override
    {
        int third = getWidth() / 3;
        depthSlider.setBounds(20, 50, 65, 75);
        amountSlider.setBounds(getWidth() - 85, 50, 65, 75);

        speedSlider.setBounds(5, 150, third - 10, 75);
        bassSlider.setBounds(third + 5, 150, third - 10, 75);
        trebleSlider.setBounds(third * 2 + 5, 150, third - 10, 75);
    }

private:
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
