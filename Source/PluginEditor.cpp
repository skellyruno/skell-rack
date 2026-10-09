#include "PluginEditor.h"
#include "Modules/ReverserModule.h"
#include "Modules/NoiseGateModule.h"
#include "Modules/ChorusModule.h"

ModularFXAudioProcessorEditor::ModularFXAudioProcessorEditor(ModularFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    addModuleBox.addItem("Add Reverser", 1);
    addModuleBox.addItem("Add Noise Gate", 2);
    addModuleBox.addItem("Add Chorus Ensemble", 3);
    addModuleBox.onChange = [this] {
        int id = addModuleBox.getSelectedId();
        juce::ScopedLock sl(audioProcessor.processLock);
        if (id == 1) audioProcessor.activeModules.push_back(std::make_unique<ReverserModule>(audioProcessor.apvts));
        if (id == 2) audioProcessor.activeModules.push_back(std::make_unique<NoiseGateModule>(audioProcessor.apvts));
        if (id == 3) audioProcessor.activeModules.push_back(std::make_unique<ChorusModule>(audioProcessor.apvts));
        updateRackLayout();
    };
    addAndMakeVisible(addModuleBox);

    setSize(900, 380);
    updateRackLayout();
}

void ModularFXAudioProcessorEditor::updateRackLayout()
{
    for (auto& module : audioProcessor.activeModules)
    {
        addAndMakeVisible(module.get());
        module->onRemoveRequested = [this](ModuleBase* target) {
            juce::ScopedLock sl(audioProcessor.processLock);
            audioProcessor.activeModules.erase(
                std::remove_if(audioProcessor.activeModules.begin(), audioProcessor.activeModules.end(),
                               [target](const std::unique_ptr<ModuleBase>& m) { return m.get() == target; }),
                audioProcessor.activeModules.end());
            updateRackLayout();
        };

        module->onMoveRequested = [this](ModuleBase* target, int direction) {
            juce::ScopedLock sl(audioProcessor.processLock);
            auto it = std::find_if(audioProcessor.activeModules.begin(), audioProcessor.activeModules.end(),
                                   [target](const std::unique_ptr<ModuleBase>& m) { return m.get() == target; });
            if (it != audioProcessor.activeModules.end())
            {
                int idx = static_cast<int>(std::distance(audioProcessor.activeModules.begin(), it));
                int newIdx = idx + direction;
                if (newIdx >= 0 && newIdx < static_cast<int>(audioProcessor.activeModules.size()))
                {
                    std::swap(audioProcessor.activeModules[idx], audioProcessor.activeModules[newIdx]);
                    updateRackLayout();
                }
            }
        };
    }
    resized();
    repaint();
}

void ModularFXAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1E1E1E));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("Modular FX Suite", 16, 10, 200, 30, juce::Justification::left);
}

void ModularFXAudioProcessorEditor::resized()
{
    addModuleBox.setBounds(getWidth() - 160, 10, 140, 25);

    auto rackArea = getLocalBounds().removeFromBottom(getHeight() - 45).reduced(10);
    int numModules = static_cast<int>(audioProcessor.activeModules.size());
    if (numModules == 0) return;

    int modWidth = (rackArea.getWidth() - (10 * (numModules - 1))) / numModules;
    for (int i = 0; i < numModules; ++i)
    {
        audioProcessor.activeModules[i]->setBounds(rackArea.removeFromLeft(modWidth));
        rackArea.removeFromLeft(10);
    }
}