#include "PluginEditor.h"

// --- UI Implementations ---
class ReverserUI : public ModuleUIBase
{
public:
    ReverserUI(DSPModuleBase* dsp, juce::AudioProcessorValueTreeState& apvts) : ModuleUIBase("Reverser", dsp)
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

    ~ReverserUI() override
    {
        mixSlider.setLookAndFeel(nullptr);
        timeSlider.setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF121212));
        paintHeader(g, juce::Colours::white);
        g.setColour(juce::Colours::grey);
        g.setFont(juce::FontOptions(14.0f));
        g.drawText("Mix", mixSlider.getBounds().translated(0, mixSlider.getHeight() / 2 + 10), juce::Justification::centred);
        g.drawText("Time beat", timeSlider.getBounds().translated(0, timeSlider.getHeight() / 2 + 8), juce::Justification::centred);
    }

    void resized() override
    {
        resizedHeader();
        auto area = getLocalBounds().removeFromBottom(getHeight() - 40);
        mixSlider.setBounds(area.removeFromTop(120).withSizeKeepingCentre(90, 90));
        timeSlider.setBounds(area.removeFromTop(80).withSizeKeepingCentre(45, 45));
    }

private:
    juce::Slider mixSlider, timeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach, timeAttach;
    PurpleArcKnobLookAndFeel purpleStyle;
};

class NoiseGateUI : public ModuleUIBase
{
public:
    NoiseGateUI(DSPModuleBase* dsp, juce::AudioProcessorValueTreeState& apvts) : ModuleUIBase("Noise Gate", dsp)
    {
        attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(attackSlider);

        releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(releaseSlider);

        attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_ATTACK", attackSlider);
        releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_RELEASE", releaseSlider);
        threshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_THRESH", threshSlider);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF0F0F0F));
        paintHeader(g, juce::Colours::white);

        auto scopeArea = getLocalBounds().removeFromTop(180).removeFromBottom(130).toFloat();
        g.setColour(juce::Colours::white.withAlpha(0.05f));
        g.drawRect(scopeArea);

        juce::Path wave;
        wave.startNewSubPath(scopeArea.getX(), scopeArea.getCentreY());
        for (float x = scopeArea.getX(); x < scopeArea.getRight(); x += 5.0f)
        {
            float y = scopeArea.getCentreY() + std::sin(x * 0.05f) * 35.0f;
            wave.lineTo(x, y);
        }
        g.setColour(juce::Colour(0xFFFFB703));
        g.strokePath(wave, juce::PathStrokeType(1.5f));

        float threshVal = static_cast<float>(threshSlider.getValue());
        float normalizedThresh = juce::jmap(threshVal, -60.0f, 0.0f, scopeArea.getBottom(), scopeArea.getY());
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.drawDashedLine(juce::Line<float>(scopeArea.getX(), normalizedThresh, scopeArea.getRight(), normalizedThresh), nullptr, 0, 2);

        thresholdPillBounds = juce::Rectangle<float>(scopeArea.getCentreX() - 55.0f, normalizedThresh - 14.0f, 110.0f, 28.0f);
        g.setColour(juce::Colour(0xFF1E1E1E));
        g.fillRoundedRectangle(thresholdPillBounds, 14.0f);
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(thresholdPillBounds, 14.0f, 1.2f);
        g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
        g.drawText("Threshold  ↕", thresholdPillBounds, juce::Justification::centred);

        g.drawText(juce::String(threshVal, 1) + " dB", scopeArea.removeFromLeft(80).removeFromTop(30), juce::Justification::centred);

        g.setFont(juce::FontOptions(12.0f));
        g.drawText("Attack: " + juce::String(static_cast<float>(attackSlider.getValue()), 0) + " ms", 20, getHeight() - 40, 100, 20, juce::Justification::left);
        g.drawText("Release: " + juce::String(static_cast<float>(releaseSlider.getValue()), 0) + " ms", 140, getHeight() - 40, 100, 20, juce::Justification::left);
    }

    void resized() override
    {
        resizedHeader();
        attackSlider.setBounds(20, getHeight() - 25, 100, 15);
        releaseSlider.setBounds(140, getHeight() - 25, 100, 15);
    }

    void mouseDown(const juce::MouseEvent& e) override { if (thresholdPillBounds.contains(e.position)) isDraggingPill = true; }
    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (isDraggingPill)
        {
            auto scopeArea = getLocalBounds().removeFromTop(180).removeFromBottom(130).toFloat();
            float norm = juce::jlimit(0.0f, 1.0f, (e.position.y - scopeArea.getY()) / scopeArea.getHeight());
            threshSlider.setValue(juce::jmap(norm, 1.0f, 0.0f, -60.0f, 0.0f));
            repaint();
        }
    }

private:
    juce::Slider attackSlider, releaseSlider, threshSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach, releaseAttach, threshAttach;
    juce::Rectangle<float> thresholdPillBounds;
    bool isDraggingPill = false;
};

class ChorusUI : public ModuleUIBase
{
public:
    ChorusUI(DSPModuleBase* dsp, juce::AudioProcessorValueTreeState& apvts) : ModuleUIBase("Chorus Ensemble", dsp)
    {
        auto setupKnob = [this](juce::Slider& slider) {
            slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            slider.setLookAndFeel(&teardropStyle);
            addAndMakeVisible(slider);
        };

        setupKnob(depthSlider); setupKnob(amountSlider); setupKnob(speedSlider); setupKnob(bassSlider); setupKnob(trebleSlider);
        depthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_DEPTH", depthSlider);
        amountAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_AMOUNT", amountSlider);
        speedAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_SPEED", speedSlider);
        bassAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_BASS", bassSlider);
        trebleAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_TREBLE", trebleSlider);
    }

    ~ChorusUI() override
    {
        depthSlider.setLookAndFeel(nullptr); amountSlider.setLookAndFeel(nullptr);
        speedSlider.setLookAndFeel(nullptr); bassSlider.setLookAndFeel(nullptr); trebleSlider.setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF6B93AC));
        SeigaihaDrawer::drawPattern(g, getLocalBounds().toFloat(), juce::Colour(0xFF3B627C));
        paintHeader(g, juce::Colours::white);
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
        g.drawText("Depth", depthSlider.getBounds().translated(0, depthSlider.getHeight() / 2 + 5), juce::Justification::centred);
        g.drawText("Amount", amountSlider.getBounds().translated(0, amountSlider.getHeight() / 2 + 5), juce::Justification::centred);
        g.drawText("Speed", speedSlider.getBounds().translated(0, speedSlider.getHeight() / 2 + 5), juce::Justification::centred);
        g.drawText("Bass", bassSlider.getBounds().translated(0, bassSlider.getHeight() / 2 + 5), juce::Justification::centred);
        g.drawText("Treble", trebleSlider.getBounds().translated(0, trebleSlider.getHeight() / 2 + 5), juce::Justification::centred);
    }

    void resized() override
    {
        resizedHeader();
        auto topRow = getLocalBounds().removeFromTop(140).removeFromBottom(80);
        depthSlider.setBounds(topRow.removeFromLeft(getWidth() / 2).withSizeKeepingCentre(50, 50));
        amountSlider.setBounds(topRow.withSizeKeepingCentre(50, 50));

        auto bottomRow = getLocalBounds().removeFromBottom(110).removeFromTop(80);
        int third = getWidth() / 3;
        speedSlider.setBounds(bottomRow.removeFromLeft(third).withSizeKeepingCentre(42, 42));
        bassSlider.setBounds(bottomRow.removeFromLeft(third).withSizeKeepingCentre(42, 42));
        trebleSlider.setBounds(bottomRow.withSizeKeepingCentre(42, 42));
    }

private:
    juce::Slider depthSlider, amountSlider, speedSlider, bassSlider, trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach, amountAttach, speedAttach, bassAttach, trebleAttach;
    TeardropKnobLookAndFeel teardropStyle;
};

// --- Editor Core ---
ModularFXAudioProcessorEditor::ModularFXAudioProcessorEditor(ModularFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    addModuleBox.addItem("Add Reverser", 1);
    addModuleBox.addItem("Add Noise Gate", 2);
    addModuleBox.addItem("Add Chorus Ensemble", 3);
    addModuleBox.onChange = [this] {
        int id = addModuleBox.getSelectedId();
        juce::ScopedLock sl(audioProcessor.processLock);
        if (id == 1) audioProcessor.dspChain.push_back(std::make_unique<ReverserDSP>(audioProcessor.apvts));
        if (id == 2) audioProcessor.dspChain.push_back(std::make_unique<NoiseGateDSP>(audioProcessor.apvts));
        if (id == 3) audioProcessor.dspChain.push_back(std::make_unique<ChorusDSP>(audioProcessor.apvts));
        rebuildUIFromDSP();
    };
    addAndMakeVisible(addModuleBox);

    setSize(900, 380);
    rebuildUIFromDSP();
}

void ModularFXAudioProcessorEditor::rebuildUIFromDSP()
{
    uiModules.clear();

    for (auto& dsp : audioProcessor.dspChain)
    {
        std::unique_ptr<ModuleUIBase> ui;
        if (dsp->getType() == ModuleType::Reverser) ui = std::make_unique<ReverserUI>(dsp.get(), audioProcessor.apvts);
        else if (dsp->getType() == ModuleType::NoiseGate) ui = std::make_unique<NoiseGateUI>(dsp.get(), audioProcessor.apvts);
        else if (dsp->getType() == ModuleType::Chorus) ui = std::make_unique<ChorusUI>(dsp.get(), audioProcessor.apvts);

        if (ui)
        {
            ui->onRemoveRequested = [this](ModuleUIBase* target) {
                juce::ScopedLock sl(audioProcessor.processLock);
                auto dspTarget = target->getDSP();
                audioProcessor.dspChain.erase(
                    std::remove_if(audioProcessor.dspChain.begin(), audioProcessor.dspChain.end(),
                                   [dspTarget](const std::unique_ptr<DSPModuleBase>& m) { return m.get() == dspTarget; }),
                    audioProcessor.dspChain.end());
                rebuildUIFromDSP();
            };

            ui->onMoveRequested = [this](ModuleUIBase* target, int direction) {
                juce::ScopedLock sl(audioProcessor.processLock);
                auto dspTarget = target->getDSP();
                auto it = std::find_if(audioProcessor.dspChain.begin(), audioProcessor.dspChain.end(),
                                       [dspTarget](const std::unique_ptr<DSPModuleBase>& m) { return m.get() == dspTarget; });
                if (it != audioProcessor.dspChain.end())
                {
                    int idx = static_cast<int>(std::distance(audioProcessor.dspChain.begin(), it));
                    int newIdx = idx + direction;
                    if (newIdx >= 0 && newIdx < static_cast<int>(audioProcessor.dspChain.size()))
                    {
                        std::swap(audioProcessor.dspChain[idx], audioProcessor.dspChain[newIdx]);
                        rebuildUIFromDSP();
                    }
                }
            };

            addAndMakeVisible(ui.get());
            uiModules.push_back(std::move(ui));
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
    addModuleBox.setBounds(getWidth() - 160, 10, 140, 25);

    auto rackArea = getLocalBounds().removeFromBottom(getHeight() - 45).reduced(10);
    int numModules = static_cast<int>(uiModules.size());
    if (numModules == 0) return;

    int modWidth = (rackArea.getWidth() - (10 * (numModules - 1))) / numModules;
    for (int i = 0; i < numModules; ++i)
    {
        uiModules[i]->setBounds(rackArea.removeFromLeft(modWidth));
        rackArea.removeFromLeft(10);
    }
}
