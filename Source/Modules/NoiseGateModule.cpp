#include "NoiseGateModule.h"

NoiseGateModule::NoiseGateModule(juce::AudioProcessorValueTreeState& vts)
    : ModuleBase("Noise Gate", ModuleType::NoiseGate, vts)
{
    attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(attackSlider);

    releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(releaseSlider);

    attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_ATTACK", attackSlider);
    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GATE_RELEASE", releaseSlider);
}

void NoiseGateModule::prepareToPlay(double, int) {}

void NoiseGateModule::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    if (getIsBypassed()) return;

    float threshLinear = juce::Decibels::decibelsToGain(thresholdDb);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float level = std::abs(buffer.getSample(0, i));
        currentEnvelope = level > threshLinear ? level : currentEnvelope * 0.99f;
        float gain = currentEnvelope > threshLinear ? 1.0f : 0.0f;

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample(ch, i, buffer.getSample(ch, i) * gain);
    }
}

void NoiseGateModule::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF0F0F0F)); // Dark scope background
    paintHeader(g, juce::Colours::white);

    // Waveform Scope Canvas Area
    auto scopeArea = getLocalBounds().removeFromTop(180).removeFromBottom(130).toFloat();
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawRect(scopeArea);

    // Simulated Dynamic Waveform
    juce::Path wave;
    wave.startNewSubPath(scopeArea.getX(), scopeArea.getCentreY());
    for (float x = scopeArea.getX(); x < scopeArea.getRight(); x += 5.0f)
    {
        float y = scopeArea.getCentreY() + std::sin(x * 0.05f) * 35.0f;
        wave.lineTo(x, y);
    }
    g.setColour(juce::Colour(0xFFFFB703)); // Gold scope line
    g.strokePath(wave, juce::PathStrokeType(1.5f));

    // Interactive Threshold Drag Line and Pill
    float normalizedThresh = juce::jmap(thresholdDb, -60.0f, 0.0f, scopeArea.getBottom(), scopeArea.getY());
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawDashedLine(juce::Line<float>(scopeArea.getX(), normalizedThresh, scopeArea.getRight(), normalizedThresh), nullptr, 0, 2);

    thresholdPillBounds = juce::Rectangle<float>(scopeArea.getCentreX() - 55, normalizedThresh - 14, 110, 28);
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRoundedRectangle(thresholdPillBounds, 14.0f);
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(thresholdPillBounds, 14.0f, 1.2f);
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawText("Threshold  ↕", thresholdPillBounds, juce::Justification::centred);

    // Dynamic dB Readout display on scope
    g.drawText(juce::String(thresholdDb, 1) + " dB", scopeArea.removeFromLeft(80).removeFromTop(30), juce::Justification::centred);

    // Bottom Analog Controls
    g.setFont(juce::FontOptions(12.0f));
    g.drawText("Attack: " + juce::String(attackSlider.getValue(), 0) + " ms", 20, getHeight() - 40, 100, 20, juce::Justification::left);
    g.drawText("Release: " + juce::String(releaseSlider.getValue(), 0) + " ms", 140, getHeight() - 40, 100, 20, juce::Justification::left);
}

void NoiseGateModule::resized()
{
    resizedHeader();
    attackSlider.setBounds(20, getHeight() - 25, 100, 15);
    releaseSlider.setBounds(140, getHeight() - 25, 100, 15);
}

void NoiseGateModule::mouseDown(const juce::MouseEvent& e)
{
    if (thresholdPillBounds.contains(e.position)) isDraggingPill = true;
}

void NoiseGateModule::mouseDrag(const juce::MouseEvent& e)
{
    if (isDraggingPill)
    {
        auto scopeArea = getLocalBounds().removeFromTop(180).removeFromBottom(130).toFloat();
        float norm = juce::jlimit(0.0f, 1.0f, (e.position.y - scopeArea.getY()) / scopeArea.getHeight());
        thresholdDb = juce::jmap(norm, 1.0f, 0.0f, -60.0f, 0.0f);
        repaint();
    }
}