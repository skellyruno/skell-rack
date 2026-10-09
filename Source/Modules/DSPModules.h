#pragma once
#include <JuceHeader.h>

enum class ModuleType { Reverser, NoiseGate, Chorus };

// Pure C++ DSP Base (No juce::Component)
class DSPModuleBase
{
public:
    DSPModuleBase(ModuleType t) : type(t) {}
    virtual ~DSPModuleBase() = default;

    virtual void prepareToPlay(double sampleRate, int samplesPerBlock) = 0;
    virtual void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) = 0;

    void setBypassed(bool bypass) { bypassed.store(bypass); }
    bool isBypassed() const { return bypassed.load(); }
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
        currentSampleRate = (sampleRate > 0.0) ? sampleRate : 44100.0;
        circularBuffer.setSize(2, static_cast<int>(currentSampleRate * 2.0));
        circularBuffer.clear();
        writePos = 0;
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
    {
        if (isBypassed()) return;

        auto* mixParam = apvts.getRawParameterValue("REV_MIX");
        float mix = mixParam ? mixParam->load() : 0.5f;

        int bufSize = circularBuffer.getNumSamples();
        if (bufSize <= 0) return;

        int grainSize = static_cast<int>(currentSampleRate * 0.25);
        if (grainSize <= 0) grainSize = 1000;

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);
            auto* ringData = circularBuffer.getWritePointer(ch);

            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                float dry = channelData[i];
                ringData[writePos] = dry;

                int offsetWithinGrain = writePos % grainSize;
                int readPos = (writePos - offsetWithinGrain) + (grainSize - offsetWithinGrain - 1);
                readPos = ((readPos % bufSize) + bufSize) % bufSize;

                float wet = ringData[readPos];
                channelData[i] = dry * (1.0f - mix) + wet * mix;

                if (ch == buffer.getNumChannels() - 1)
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

        auto* threshParam = apvts.getRawParameterValue("GATE_THRESH");
        float threshDb = threshParam ? threshParam->load() : -30.0f;
        float threshLinear = juce::Decibels::decibelsToGain(threshDb);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float level = std::abs(buffer.getSample(0, i));
            currentEnvelope = level > threshLinear ? level : currentEnvelope * 0.99f;
            float gain = currentEnvelope > threshLinear ? 1.0f : 0.0f;

            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
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