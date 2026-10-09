#include "PluginEditor.h"

ModularFXAudioProcessorEditor::ModularFXAudioProcessorEditor(ModularFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      reverserCard(p.apvts), noiseGateCard(p.apvts), chorusCard(p.apvts)
{
    addAndMakeVisible(reverserCard);
    addAndMakeVisible(noiseGateCard);
    addAndMakeVisible(chorusCard);

    setSize(850, 320);
}

void ModularFXAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1E1E1E));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("Modular FX Suite", 16, 10, 300, 30, juce::Justification::left);
}

void ModularFXAudioProcessorEditor::resized()
{
    auto rackArea = getLocalBounds().removeFromBottom(getHeight() - 45).reduced(10);
    int modWidth = (rackArea.getWidth() - 20) / 3;

    reverserCard.setBounds(rackArea.removeFromLeft(modWidth));
    rackArea.removeFromLeft(10);

    noiseGateCard.setBounds(rackArea.removeFromLeft(modWidth));
    rackArea.removeFromLeft(10);

    chorusCard.setBounds(rackArea.removeFromLeft(modWidth));
}
