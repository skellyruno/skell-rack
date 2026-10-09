#include "PluginEditor.h"

ModularFXAudioProcessorEditor::ModularFXAudioProcessorEditor(ModularFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    addModuleBox.addItem("+ Add Reverser", ID_Reverser);
    addModuleBox.addItem("+ Add Noise Gate", ID_NoiseGate);
    addModuleBox.addItem("+ Add Chorus Ensemble", ID_Chorus);
    addModuleBox.onChange = [this] {
        int id = addModuleBox.getSelectedId();
        if (id <= 0) return;

        auto currentOrder = audioProcessor.getModuleOrder();
        if (std::find(currentOrder.begin(), currentOrder.end(), id) == currentOrder.end())
        {
            currentOrder.push_back(id);
            audioProcessor.updateModuleOrder(currentOrder);
        }

        triggerUIRebuild();
    };
    addAndMakeVisible(addModuleBox);

    setSize(880, 340);
    rebuildRack();
}

void ModularFXAudioProcessorEditor::triggerUIRebuild()
{
    juce::Component::SafePointer<ModularFXAudioProcessorEditor> safeThis(this);
    juce::MessageManager::callAsync([safeThis]() {
        if (safeThis != nullptr)
        {
            safeThis->addModuleBox.setSelectedId(0, juce::dontSendNotification);
            safeThis->rebuildRack();
        }
    });
}

void ModularFXAudioProcessorEditor::rebuildRack()
{
    cards.clear();

    auto currentOrder = audioProcessor.getModuleOrder();
    for (int modId : currentOrder)
    {
        std::unique_ptr<ModuleCardBase> card;
        if (modId == ID_Reverser) card = std::make_unique<ReverserCard>(audioProcessor);
        else if (modId == ID_NoiseGate) card = std::make_unique<NoiseGateCard>(audioProcessor);
        else if (modId == ID_Chorus) card = std::make_unique<ChorusCard>(audioProcessor);

        if (card != nullptr)
        {
            card->onMoveRequested = [this](int id, int direction) {
                auto order = audioProcessor.getModuleOrder();
                auto it = std::find(order.begin(), order.end(), id);
                if (it != order.end())
                {
                    int idx = static_cast<int>(std::distance(order.begin(), it));
                    int newIdx = idx + direction;
                    if (newIdx >= 0 && newIdx < static_cast<int>(order.size()))
                    {
                        std::swap(order[idx], order[newIdx]);
                        audioProcessor.updateModuleOrder(order);
                    }
                }
                triggerUIRebuild();
            };

            card->onRemoveRequested = [this](int id) {
                auto order = audioProcessor.getModuleOrder();
                order.erase(std::remove(order.begin(), order.end(), id), order.end());
                audioProcessor.updateModuleOrder(order);
                triggerUIRebuild();
            };

            addAndMakeVisible(card.get());
            cards.push_back(std::move(card));
        }
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
    addModuleBox.setBounds(getWidth() - 170, 10, 150, 24);

    auto rackArea = getLocalBounds().removeFromBottom(getHeight() - 45).reduced(10);
    int numModules = static_cast<int>(cards.size());
    if (numModules == 0) return;

    int modWidth = (rackArea.getWidth() - (10 * (numModules - 1))) / numModules;
    for (int i = 0; i < numModules; ++i)
    {
        cards[i]->setBounds(rackArea.removeFromLeft(modWidth));
        rackArea.removeFromLeft(10);
    }
}
