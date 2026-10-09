#pragma once

#include "ModuleBase.h"
#include "../CustomLookAndFeel.h"

class ChorusModule : public ModuleBase
{
public:
    ChorusModule(juce::AudioProcessorValueTreeState& vts);
    ~ChorusModule() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider depthSlider, amountSlider, speedSlider, bassSlider, trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach, amountAttach, speedAttach, bassAttach, trebleAttach;

    TeardropKnobLookAndFeel teardropStyle;
};
