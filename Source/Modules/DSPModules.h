#pragma once
#include <JuceHeader.h>

enum class ModuleType { Reverser, NoiseGate, Chorus };

class DSPModuleBase
{
public:
    DSPModuleBase(ModuleType t) : type(t) {}
    virtual ~DSPModuleBase() = default;

    virtual void prepareToPlay(double sampleRate, int samplesPerBlock) = 0;
    virtual void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) = 0;

    void setBypassed(bool bypass) { bypassed.store(bypass, std::memory_order_relaxed); }
    bool isBypassed() const { return bypassed.load(std::memory_order_relaxed); }
    ModuleType getType() const { return type; }

protected:
    ModuleType type;
    std::atomic<bool> bypassed { false };
};

// --- Reverser DSP ---
class ReverserDSP : public DSPModuleBase
{
public:
    ReverserDSP(juce::AudioProcessorValueTreeState& vts) 
        : DSPModuleBase(ModuleType::Reverser), apvts(vts) {}

    void prepareToPlay(double sampleRate, int) override
    {
        currentSampleRate = (sampleRate > 1000.0) ? sampleRate : 44100.0;
        int bufSamples = static_cast<int>(currentSampleRate * 2.0);
        if (bufSamples <= 0) bufSamples = 88200;

        circularBuffer.setSize(2, bufSamples, false, true, true);
        circularBuffer.clear();
        writePos = 0;
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
    {
        if (isBypassed()) return;

        int numInputChannels = buffer.getNumChannels();
        int numSamples = buffer.getNumSamples();
        int bufSize = circularBuffer.getNumSamples();

        if (numInputChannels == 0 || numSamples == 0 || bufSize <= 0) return;

        auto* mixParam = apvts.getRawParameterValue("REV_MIX");
        float mix = mixParam ? mixParam->load() : 0.5f;

        int processChannels = std::min(numInputChannels, circularBuffer.getNumChannels());
        int grainSize = static_cast<int>(currentSampleRate * 0.25);
        if (grainSize <= 0) grainSize = 1000;

        for (int ch = 0; ch < processChannels; ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);
            auto* ringData = circularBuffer.getWritePointer(ch);

            for (int i = 0; i < numSamples; ++i)
            {
                float dry = channelData[i];
                ringData[writePos] = dry;

                int offsetWithinGrain = writePos % grainSize;
                int readPos = (writePos - offsetWithinGrain) + (grainSize - offsetWithinGrain - 1);
                readPos = ((readPos % bufSize) + bufSize) % bufSize;

                float wet = ringData[readPos];
                channelData[i] = dry * (1.0f - mix) + wet * mix;

                if (ch == processChannels - 1)
                    writePos = (writePos + 1) % bufSize;
            }
        }
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::AudioBuffer<float> circularBuffer;
    int writePos = 0;
    double currentSampleRate = 44100.0;
};

// --- Noise Gate DSP ---
class NoiseGateDSP : public DSPModuleBase
{
public:
    NoiseGateDSP(juce::AudioProcessorValueTreeState& vts) 
        : DSPModuleBase(ModuleType::NoiseGate), apvts(vts) {}

    void prepareToPlay(double, int) override {}

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
    {
        if (isBypassed()) return;

        int numChannels = buffer.getNumChannels();
        int numSamples = buffer.getNumSamples();
        if (numChannels == 0 || numSamples == 0) return;

        auto* threshParam = apvts.getRawParameterValue("GATE_THRESH");
        float threshDb = threshParam ? threshParam->load() : -30.0f;
        float threshLinear = juce::Decibels::decibelsToGain(threshDb);

        for (int i = 0; i < numSamples; ++i)
        {
            float level = std::abs(buffer.getSample(0, i));
            currentEnvelope = level > threshLinear ? level : currentEnvelope * 0.99f;
            float gain = currentEnvelope > threshLinear ? 1.0f : 0.0f;

            for (int ch = 0; ch < numChannels; ++ch)
                buffer.setSample(ch, i, buffer.getSample(ch, i) * gain);
        }
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    float currentEnvelope = 0.0f;
};

// --- Chorus DSP ---
class ChorusDSP : public DSPModuleBase
{
public:
    ChorusDSP(juce::AudioProcessorValueTreeState& vts) 
        : DSPModuleBase(ModuleType::Chorus), apvts(vts) {}

    void prepareToPlay(double, int) override {}

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override
    {
        if (isBypassed()) return;
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
};
