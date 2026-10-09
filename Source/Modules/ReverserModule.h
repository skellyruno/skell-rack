#pragma once
#include "ModuleBase.h"
#include "../CustomLookAndFeel.h"

class ReverserModule : public ModuleBase
{
public:
    ReverserModule(juce::AudioProcessorValueTreeState& vts);
    ~ReverserModule() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider mixSlider;
    juce::Slider timeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeAttach;

    PurpleArcKnobLookAndFeel purpleStyle;
    
    juce::AudioBuffer<float> circularBuffer;
    int writePos = 0;
    double currentSampleRate = 44100.0;
};