#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

class ModuleCardBase : public juce::Component
{
public:
    ModuleCardBase(const juce::String& title, int id)
        : moduleTitle(title), moduleId(id)
    {
        bypassButton.setClickingTogglesState(true);
        bypassButton.setToggleState(true, juce::dontSendNotification);
        addAndMakeVisible(bypassButton);

        moveLeftButton.setButtonText("<");
        addAndMakeVisible(moveLeftButton);

        moveRightButton.setButtonText(">");
        addAndMakeVisible(moveRightButton);

        removeButton.setButtonText("X");
        addAndMakeVisible(removeButton);
    }

    virtual ~ModuleCardBase() override = default;

    std::function<void(int, int)> onMoveRequested;
    std::function<void(int)> onRemoveRequested;

    int getModuleID() const { return moduleId; }

protected:
    void paintHeader(juce::Graphics& g, juce::Colour textColour)
    {
        g.setColour(textColour);
        g.setFont(juce::FontOptions(15.0f, juce::Font::bold));
        g.drawText(moduleTitle, 55, 6, 120, 24, juce::Justification::left);

        g.setColour(textColour.withAlpha(0.25f));
        int w = getWidth(), h = getHeight();
        if (w > 20 && h > 20)
        {
            g.fillRect(3, 3, 6, 6);
            g.fillRect(w - 9, 3, 6, 6);
            g.fillRect(3, h - 9, 6, 6);
            g.fillRect(w - 9, h - 9, 6, 6);
        }
    }

    void resizedHeader()
    {
        bypassButton.setBounds(12, 6, 36, 22);
        moveLeftButton.setBounds(getWidth() - 80, 6, 22, 22);
        moveRightButton.setBounds(getWidth() - 55, 6, 22, 22);
        removeButton.setBounds(getWidth() - 30, 6, 22, 22);
    }

    void setupHeaderCallbacks()
    {
        moveLeftButton.onClick = [this] { if (onMoveRequested) onMoveRequested(moduleId, -1); };
        moveRightButton.onClick = [this] { if (onMoveRequested) onMoveRequested(moduleId, 1); };
        removeButton.onClick = [this] { if (onRemoveRequested) onRemoveRequested(moduleId); };
    }

    juce::String moduleTitle;
    int moduleId;
    juce::ToggleButton bypassButton;
    juce::TextButton moveLeftButton, moveRightButton, removeButton;
};

// --- Reverser Card UI ---
class ReverserCard : public ModuleCardBase
{
public:
    ReverserCard(ModularFXAudioProcessor& p)
        : ModuleCardBase("Reverser", ID_Reverser), processor(p)
    {
        setupHeaderCallbacks();

        mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        mixSlider.setLookAndFeel(&purpleStyle);
        addAndMakeVisible(mixSlider);

        timeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        timeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        timeSlider.setLookAndFeel(&purpleStyle);
        addAndMakeVisible(timeSlider);

        mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "REV_MIX", mixSlider);
        timeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "REV_TIME", timeSlider);

        bypassButton.onClick = [this] {
            processor.reverser.setBypassed(!bypassButton.getToggleState());
        };
    }

    ~ReverserCard() override
    {
        mixSlider.setLookAndFeel(nullptr);
        timeSlider.setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF121212));
        paintHeader(g, juce::Colours::white);

        g.setColour(juce::Colours::grey);
        g.setFont(juce::FontOptions(13.0f));
        g.drawText("Mix", mixSlider.getX(), mixSlider.getBottom() + 2, mixSlider.getWidth(), 18, juce::Justification::centred);
        g.drawText("Time", timeSlider.getX(), timeSlider.getBottom() + 2, timeSlider.getWidth(), 18, juce::Justification::centred);
    }

    void resized() override
    {
        resizedHeader();
        mixSlider.setBounds(getWidth() / 2 - 40, 48, 80, 80);
        timeSlider.setBounds(getWidth() / 2 - 25, 168, 50, 50);
    }

private:
    ModularFXAudioProcessor& processor;
    PurpleArcKnobLookAndFeel purpleStyle;
    juce::Slider mixSlider, timeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach, timeAttach;
};

// --- Noise Gate Card UI ---
class NoiseGateCard : public ModuleCardBase
{
public:
    NoiseGateCard(ModularFXAudioProcessor& p)
        : ModuleCardBase("Noise Gate", ID_NoiseGate), processor(p)
    {
        setupHeaderCallbacks();

        attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        addAndMakeVisible(attackSlider);

        releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        addAndMakeVisible(releaseSlider);

        threshSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        threshSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        addAndMakeVisible(threshSlider);

        attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "GATE_ATTACK", attackSlider);
        releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "GATE_RELEASE", releaseSlider);
        threshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "GATE_THRESH", threshSlider);

        bypassButton.onClick = [this] {
            processor.noiseGate.setBypassed(!bypassButton.getToggleState());
        };
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF0F0F0F));
        paintHeader(g, juce::Colours::white);

        auto bounds = getLocalBounds();
        if (bounds.getHeight() < 180) return;

        auto scopeArea = bounds.removeFromTop(180).removeFromBottom(130).toFloat();
        if (scopeArea.getHeight() <= 0.0f) return;

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
        g.drawText("Attack", attackSlider.getX(), attackSlider.getY() - 16, attackSlider.getWidth(), 16, juce::Justification::centred);
        g.drawText("Release", releaseSlider.getX(), releaseSlider.getY() - 16, releaseSlider.getWidth(), 16, juce::Justification::centred);
    }

    void resized() override
    {
        resizedHeader();
        int w = getWidth() - 30;
        threshSlider.setBounds(15, 45, w, 35);
        attackSlider.setBounds(15, 130, w, 45);
        releaseSlider.setBounds(15, 200, w, 45);
    }

    void mouseDown(const juce::MouseEvent& e) override { if (thresholdPillBounds.contains(e.position)) isDraggingPill = true; }
    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (isDraggingPill)
        {
            auto bounds = getLocalBounds();
            if (bounds.getHeight() < 180) return;
            auto scopeArea = bounds.removeFromTop(180).removeFromBottom(130).toFloat();
            if (scopeArea.getHeight() <= 0.0f) return;

            float norm = juce::jlimit(0.0f, 1.0f, (e.position.y - scopeArea.getY()) / scopeArea.getHeight());
            threshSlider.setValue(juce::jmap(norm, 1.0f, 0.0f, -60.0f, 0.0f));
            repaint();
        }
    }

private:
    ModularFXAudioProcessor& processor;
    juce::Slider attackSlider, releaseSlider, threshSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach, releaseAttach, threshAttach;
    juce::Rectangle<float> thresholdPillBounds;
    bool isDraggingPill = false;
};

// --- Chorus Ensemble Card UI ---
class ChorusCard : public ModuleCardBase
{
public:
    ChorusCard(ModularFXAudioProcessor& p)
        : ModuleCardBase("Chorus Ensemble", ID_Chorus), processor(p)
    {
        setupHeaderCallbacks();

        auto setupKnob = [this](juce::Slider& slider) {
            slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            slider.setLookAndFeel(&teardropStyle);
            addAndMakeVisible(slider);
        };

        setupKnob(depthSlider); setupKnob(amountSlider); setupKnob(speedSlider); setupKnob(bassSlider); setupKnob(trebleSlider);
        depthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "CHO_DEPTH", depthSlider);
        amountAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "CHO_AMOUNT", amountSlider);
        speedAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "CHO_SPEED", speedSlider);
        bassAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "CHO_BASS", bassSlider);
        trebleAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "CHO_TREBLE", trebleSlider);

        bypassButton.onClick = [this] {
            processor.chorus.setBypassed(!bypassButton.getToggleState());
        };
    }

    ~ChorusCard() override
    {
        depthSlider.setLookAndFeel(nullptr); amountSlider.setLookAndFeel(nullptr);
        speedSlider.setLookAndFeel(nullptr); bassSlider.setLookAndFeel(nullptr); trebleSlider.setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF6B93AC));
        SeigaihaDrawer::drawPattern(g, getLocalBounds().toFloat(), juce::Colour(0xFF3B627C));
        paintHeader(g, juce::Colours::white);
    }

    void resized() override
    {
        resizedHeader();
        int third = getWidth() / 3;
        depthSlider.setBounds(20, 50, 55, 55);
        amountSlider.setBounds(getWidth() - 75, 50, 55, 55);

        speedSlider.setBounds(5, 150, third - 10, 50);
        bassSlider.setBounds(third + 5, 150, third - 10, 50);
        trebleSlider.setBounds(third * 2 + 5, 150, third - 10, 50);
    }

private:
    ModularFXAudioProcessor& processor;
    TeardropKnobLookAndFeel teardropStyle;
    juce::Slider depthSlider, amountSlider, speedSlider, bassSlider, trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach, amountAttach, speedAttach, bassAttach, trebleAttach;
};

// --- Main Editor ---
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

    ReverserCard reverserCard;
    NoiseGateCard noiseGateCard;
    ChorusCard chorusCard;

    void updateCardCallbacks();
};
