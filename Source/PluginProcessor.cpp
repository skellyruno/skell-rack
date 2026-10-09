#include "PluginProcessor.h"
#include "PluginEditor.h"

ModularFXAudioProcessor::ModularFXAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    gainParam = apvts.getRawParameterValue("GAIN");
}

bool ModularFXAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    auto mainInput = layouts.getMainInputChannelSet();
    auto mainOutput = layouts.getMainOutputChannelSet();

    if (mainOutput != juce::AudioChannelSet::mono() && mainOutput != juce::AudioChannelSet::stereo())
        return false;

    return mainInput == mainOutput;
}

juce::AudioProcessorValueTreeState::ParameterLayout ModularFXAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Master Gain", 0.0f, 1.0f, 0.8f));
    return { params.begin(), params.end() };
}

void ModularFXAudioProcessor::prepareToPlay(double, int) {}

void ModularFXAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float currentGain = gainParam ? gainParam->load() : 1.0f;

    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        buffer.applyGain(ch, 0, buffer.getNumSamples(), currentGain);
    }
}

juce::AudioProcessorEditor* ModularFXAudioProcessor::createEditor()
{
    return new ModularFXAudioProcessorEditor(*this);
}

void ModularFXAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml != nullptr)
        copyXmlToBinary(*xml, destData);
}

void ModularFXAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (data == nullptr || sizeInBytes <= 0) return;

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ModularFXAudioProcessor();
}
