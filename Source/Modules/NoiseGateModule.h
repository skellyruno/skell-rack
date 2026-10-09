#pragma once

#include "ModuleBase.h"

class NoiseGateModule : public ModuleBase
{
public:
    NoiseGateModule(juce::AudioProcessorValueTreeState& vts);
    ~NoiseGateModule() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttach;

    float thresholdDb = -30.0f;
    float currentEnvelope = 0.0f;
    juce::Rectangle<float> thresholdPillBounds;
    bool isDraggingPill = false;
};
