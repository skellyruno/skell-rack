#include "PluginProcessor.h"
#include "PluginEditor.h"

ModularFXAudioProcessor::ModularFXAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    dspChain.push_back(std::make_unique<ReverserDSP>(apvts));
    dspChain.push_back(std::make_unique<NoiseGateDSP>(apvts));
    dspChain.push_back(std::make_unique<ChorusDSP>(apvts));
}

juce::AudioProcessorValueTreeState::ParameterLayout ModularFXAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("REV_MIX", "Reverser Mix", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("REV_TIME", "Reverser Time", 0.0f, 1.0f, 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GATE_THRESH", "Gate Threshold", -60.0f, 0.0f, -30.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GATE_ATTACK", "Gate Attack", 1.0f, 100.0f, 37.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GATE_RELEASE", "Gate Release", 10.0f, 1000.0f, 200.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CHO_DEPTH", "Chorus Depth", 0.0f, 1.0f, 0.6f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CHO_AMOUNT", "Chorus Amount", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CHO_SPEED", "Chorus Speed", 0.1f, 10.0f, 1.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CHO_BASS", "Chorus Bass", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CHO_TREBLE", "Chorus Treble", -12.0f, 12.0f, 0.0f));

    return { params.begin(), params.end() };
}

void ModularFXAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ScopedLock sl(processLock);
    for (auto& dsp : dspChain)
        dsp->prepareToPlay(sampleRate, samplesPerBlock);
}

void ModularFXAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedLock sl(processLock);
    for (auto& dsp : dspChain)
    {
        dsp->processBlock(buffer, midi);
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
    copyXmlToBinary(*xml, destData);
}

void ModularFXAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ModularFXAudioProcessor();
}
