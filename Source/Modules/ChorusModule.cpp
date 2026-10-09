#include "ChorusModule.h"

ChorusModule::ChorusModule(juce::AudioProcessorValueTreeState& vts)
    : ModuleBase("Chorus Ensemble", ModuleType::Chorus, vts)
{
    auto setupKnob = [this](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setLookAndFeel(&teardropStyle);
        addAndMakeVisible(slider);
    };

    setupKnob(depthSlider);
    setupKnob(amountSlider);
    setupKnob(speedSlider);
    setupKnob(bassSlider);
    setupKnob(trebleSlider);

    depthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_DEPTH", depthSlider);
    amountAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_AMOUNT", amountSlider);
    speedAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_SPEED", speedSlider);
    bassAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_BASS", bassSlider);
    trebleAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "CHO_TREBLE", trebleSlider);
}

ChorusModule::~ChorusModule()
{
    depthSlider.setLookAndFeel(nullptr);
    amountSlider.setLookAndFeel(nullptr);
    speedSlider.setLookAndFeel(nullptr);
    bassSlider.setLookAndFeel(nullptr);
    trebleSlider.setLookAndFeel(nullptr);
}

void ChorusModule::prepareToPlay(double, int) {}

void ChorusModule::processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&)
{
    if (getIsBypassed()) return;
    // Chorus DSP processing implementation
}

void ChorusModule::paint(juce::Graphics& g)
{
    // Slate-Blue module background
    g.fillAll(juce::Colour(0xFF6B93AC));

    // Japanese Seigaiha vector wave pattern graphics
    SeigaihaDrawer::drawPattern(g, getLocalBounds().toFloat(), juce::Colour(0xFF3B627C));

    paintHeader(g, juce::Colours::white);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    
    // Labels corresponding to white teardrop knobs
    g.drawText("Depth", depthSlider.getBounds().translated(0, depthSlider.getHeight() / 2 + 5), juce::Justification::centred);
    g.drawText("Amount", amountSlider.getBounds().translated(0, amountSlider.getHeight() / 2 + 5), juce::Justification::centred);
    g.drawText("Speed", speedSlider.getBounds().translated(0, speedSlider.getHeight() / 2 + 5), juce::Justification::centred);
    g.drawText("Bass", bassSlider.getBounds().translated(0, bassSlider.getHeight() / 2 + 5), juce::Justification::centred);
    g.drawText("Treble", trebleSlider.getBounds().translated(0, trebleSlider.getHeight() / 2 + 5), juce::Justification::centred);
}

void ChorusModule::resized()
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