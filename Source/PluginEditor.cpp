#include "PluginEditor.h"

ModularFXAudioProcessorEditor::ModularFXAudioProcessorEditor(ModularFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    revMixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    revMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(revMixSlider);

    gateThreshSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gateThreshSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(gateThreshSlider);

    choDepthSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    choDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(choDepthSlider);

    revMixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "REV_MIX", revMixSlider);
    gateThreshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GATE_THRESH", gateThreshSlider);
    choDepthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "CHO_DEPTH", choDepthSlider);

    setSize(600, 300);
}

void ModularFXAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1E1E1E));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("Modular FX Suite - Step A (Fixed DSP)", getLocalBounds().removeFromTop(40), juce::Justification::centred);

    g.setFont(juce::FontOptions(12.0f));
    g.drawText("Reverser", 50, 230, 100, 20, juce::Justification::centred);
    g.drawText("Gate Thresh", 250, 230, 100, 20, juce::Justification::centred);
    g.drawText("Chorus Depth", 450, 230, 100, 20, juce::Justification::centred);
}

void ModularFXAudioProcessorEditor::resized()
{
    revMixSlider.setBounds(50, 80, 100, 140);
    gateThreshSlider.setBounds(250, 80, 100, 140);
    choDepthSlider.setBounds(450, 80, 100, 140);
}
