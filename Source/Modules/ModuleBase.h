#pragma once
#include <JuceHeader.h>

enum class ModuleType
{
    Reverser,
    NoiseGate,
    Chorus
};

class ModuleBase : public juce::Component
{
public:
    ModuleBase(const juce::String& moduleName, ModuleType type, juce::AudioProcessorValueTreeState& vts)
        : name(moduleName), moduleType(type), apvts(vts)
    {
        bypassButton.setClickingTogglesState(true);
        bypassButton.setToggleState(true, juce::dontSendNotification);
        bypassButton.onClick = [this] { isBypassed = !bypassButton.getToggleState(); };
        addAndMakeVisible(bypassButton);

        removeButton.setButtonText("X");
        removeButton.onClick = [this] { if (onRemoveRequested) onRemoveRequested(this); };
        addAndMakeVisible(removeButton);

        moveLeftButton.setButtonText("<");
        moveLeftButton.onClick = [this] { if (onMoveRequested) onMoveRequested(this, -1); };
        addAndMakeVisible(moveLeftButton);

        moveRightButton.setButtonText(">");
        moveRightButton.onClick = [this] { if (onMoveRequested) onMoveRequested(this, 1); };
        addAndMakeVisible(moveRightButton);
    }

    virtual ~ModuleBase() override = default;

    virtual void prepareToPlay(double sampleRate, int samplesPerBlock) = 0;
    virtual void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) = 0;

    bool getIsBypassed() const { return !bypassButton.getToggleState(); }
    ModuleType getType() const { return moduleType; }
    juce::String getName() const { return name; }

    std::function<void(ModuleBase*)> onRemoveRequested;
    std::function<void(ModuleBase*, int)> onMoveRequested;

    void paintHeader(juce::Graphics& g, juce::Colour textColour)
    {
        g.setColour(textColour);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText(name, getLocalBounds().removeFromTop(36).withTrimmedLeft(60), juce::Justification::centredLeft);

        // Visual Drag Handle Indicators on 4 Corners
        g.setColour(textColour.withAlpha(0.3f));
        int w = getWidth(), h = getHeight();
        g.fillRect(2, 2, 8, 8);
        g.fillRect(w - 10, 2, 8, 8);
        g.fillRect(2, h - 10, 8, 8);
        g.fillRect(w - 10, h - 10, 8, 8);
    }

    void resizedHeader()
    {
        auto topArea = getLocalBounds().removeFromTop(36).reduced(6, 4);
        bypassButton.setBounds(topArea.removeFromLeft(32));
        moveLeftButton.setBounds(topArea.removeFromLeft(24));
        moveRightButton.setBounds(topArea.removeFromLeft(24));
        removeButton.setBounds(topArea.removeFromRight(24));
    }

protected:
    juce::String name;
    ModuleType moduleType;
    juce::AudioProcessorValueTreeState& apvts;
    bool isBypassed = false;

    juce::ToggleButton bypassButton;
    juce::TextButton removeButton;
    juce::TextButton moveLeftButton;
    juce::TextButton moveRightButton;
};