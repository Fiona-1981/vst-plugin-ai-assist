#include "PluginProcessor.h"

FionaGainAudioProcessor::FionaGainAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    gainDbParam = apvts.getRawParameterValue (gainParamId);
    jassert (gainDbParam != nullptr);
}

juce::AudioProcessorValueTreeState::ParameterLayout FionaGainAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { gainParamId, 1 },
        "Gain",
        juce::NormalisableRange<float> (-60.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    return layout;
}

void FionaGainAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    smoothedGain.reset (sampleRate, 0.02); // 20 ms ramp avoids zipper noise
    smoothedGain.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (gainDbParam->load()));
}

bool FionaGainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return out == layouts.getMainInputChannelSet();
}

// Real-time audio thread: no allocation, locks, logging or I/O in here.
void FionaGainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numInputs  = getTotalNumInputChannels();
    const auto numOutputs = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    for (auto ch = numInputs; ch < numOutputs; ++ch)
        buffer.clear (ch, 0, numSamples);

    smoothedGain.setTargetValue (juce::Decibels::decibelsToGain (gainDbParam->load()));

    if (! smoothedGain.isSmoothing())
    {
        buffer.applyGain (0, numSamples, smoothedGain.getTargetValue());
        return;
    }

    auto* const* channels = buffer.getArrayOfWritePointers();

    for (int i = 0; i < numSamples; ++i)
    {
        const auto gain = smoothedGain.getNextValue();

        for (int ch = 0; ch < numInputs; ++ch)
            channels[ch][i] *= gain;
    }
}

juce::AudioProcessorEditor* FionaGainAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

void FionaGainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void FionaGainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// JUCE's plugin wrappers call this to create the processor.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FionaGainAudioProcessor();
}
