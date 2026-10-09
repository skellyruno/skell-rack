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

void ReverserModule::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 44100.0;
    circularBuffer.setSize(2, static_cast<int>(currentSampleRate * 2.0));
    circularBuffer.clear();
    writePos = 0;
}

void ReverserModule::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    if (getIsBypassed()) return;

    // Read thread-safely from APVTS instead of GUI slider
    float mix = apvts.getRawParameterValue("REV_MIX")->load();
    int bufSize = circularBuffer.getNumSamples();
    if (bufSize <= 0) return;

    int grainSize = static_cast<int>(currentSampleRate * 0.25);
    if (grainSize <= 0) grainSize = 1000; // Guard against division by zero

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* channelData = buffer.getWritePointer(ch);
        auto* ringData = circularBuffer.getWritePointer(ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float dry = channelData[i];
            ringData[writePos] = dry;

            int offsetWithinGrain = writePos % grainSize;
            int readPos = (writePos - offsetWithinGrain) + (grainSize - offsetWithinGrain - 1);

            // Safe positive modulo wraparound
            readPos = ((readPos % bufSize) + bufSize) % bufSize;

            float wet = ringData[readPos];
            channelData[i] = dry * (1.0f - mix) + wet * mix;

            if (ch == buffer.getNumChannels() - 1)
                writePos = (writePos + 1) % bufSize;
        }
    }
}

void ReverserModule::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF121212));
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
