#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

class ModuleUIBase : public juce::Component
{
public:
    ModuleUIBase(const juce::String& name, DSPModuleBase* targetDsp)
        : moduleName(name), dsp(targetDsp)
    {
        bypassButton.setClickingTogglesState(true);
        bypassButton.setToggleState(true, juce::dontSendNotification);
        bypassButton.onClick = [this] { if (dsp) dsp->setBypassed(!bypassButton.getToggleState()); };
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

    std::function<void(ModuleUIBase*)> onRemoveRequested;
    std::function<void(ModuleUIBase*, int)> onMoveRequested;
    DSPModuleBase* getDSP() const { return dsp; }

protected:
    void paintHeader(juce::Graphics& g, juce::Colour textColour)
    {
        g.setColour(textColour);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText(moduleName, getLocalBounds().removeFromTop(36).withTrimmedLeft(60), juce::Justification::centredLeft);

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

    juce::String moduleName;
    DSPModuleBase* dsp = nullptr;
    juce::ToggleButton bypassButton;
    juce::TextButton removeButton, moveLeftButton, moveRightButton;
};

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
    std::vector<std::unique_ptr<ModuleUIBase>> uiModules;

    void rebuildUIFromDSP();
};
