#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ModularFXAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ModularFXAudioProcessorEditor(ModularFXAudioProcessor& p);
    ~ModularFXAudioProcessorEditor() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ModularFXAudioProcessor& audioProcessor;
    juce::ComboBox addModuleBox;

    void updateRackLayout();
};