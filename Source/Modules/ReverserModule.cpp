#include "ReverserModule.h"

ReverserModule::ReverserModule(juce::AudioProcessorValueTreeState& vts)
    : ModuleBase("Reverser", ModuleType::Reverser, vts)
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

ReverserModule::~ReverserModule()
{
    mixSlider.setLookAndFeel(nullptr);
    timeSlider.setLookAndFeel(nullptr);
}

void ReverserModule::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    circularBuffer.setSize(2, static_cast<int>(sampleRate * 2.0));
    circularBuffer.clear();
    writePos = 0;
}

void ReverserModule::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    if (getIsBypassed()) return;

    float mix = mixSlider.getValue();
    int grainSize = static_cast<int>(currentSampleRate * 0.25f); // Sync fraction

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* channelData = buffer.getWritePointer(ch);
        auto* ringData = circularBuffer.getWritePointer(ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float dry = channelData[i];
            ringData[writePos] = dry;

            int readPos = (writePos - (writePos % grainSize)) + (grainSize - (writePos % grainSize) - 1);
            readPos = (readPos + circularBuffer.getNumSamples()) % circularBuffer.getNumSamples();

            float wet = ringData[readPos];
            channelData[i] = dry * (1.0f - mix) + wet * mix;

            if (ch == buffer.getNumChannels() - 1)
                writePos = (writePos + 1) % circularBuffer.getNumSamples();
        }
    }
}

void ReverserModule::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF121212)); // Dark minimalist canvas
    paintHeader(g, juce::Colours::white);

    g.setColour(juce::Colours::grey);
    g.setFont(juce::FontOptions(14.0f));
    g.drawText("Mix", mixSlider.getBounds().translated(0, mixSlider.getHeight() / 2 + 10), juce::Justification::centred);
    g.drawText("Time beat", timeSlider.getBounds().translated(0, timeSlider.getHeight() / 2 + 8), juce::Justification::centred);
}

void ReverserModule::resized()
{
    resizedHeader();
    auto area = getLocalBounds().removeFromBottom(getHeight() - 40);
    mixSlider.setBounds(area.removeFromTop(120).withSizeKeepingCentre(90, 90));
    timeSlider.setBounds(area.removeFromTop(80).withSizeKeepingCentre(45, 45));
}