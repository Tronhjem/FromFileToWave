/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Envelope.h"
#include "PluginEditor.h"
#include "WaveTableFileReader.h"
#include "Defines.h"

//==============================================================================
FromFileToWaveAudioProcessor::FromFileToWaveAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), mWasDroneMode(false), mWaveTableOsc(440, 48000)
#endif
{
}

FromFileToWaveAudioProcessor::~FromFileToWaveAudioProcessor()
{
}

//==============================================================================
const juce::String FromFileToWaveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FromFileToWaveAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FromFileToWaveAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FromFileToWaveAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FromFileToWaveAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FromFileToWaveAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FromFileToWaveAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FromFileToWaveAudioProcessor::setCurrentProgram (int index)
{
    UNUSED(index);
}

const juce::String FromFileToWaveAudioProcessor::getProgramName (int index)
{
    UNUSED(index);
    return {};
}

void FromFileToWaveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    UNUSED(index);
    UNUSED(newName);
}

//==============================================================================
void FromFileToWaveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    UNUSED(samplesPerBlock);
    mSampleRate = sampleRate;
    mEnvelope.updateEnvelopeIncrements(static_cast<float>(mSampleRate));
}

void FromFileToWaveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FromFileToWaveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FromFileToWaveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if (!mDroneMode)
    {
        if (mWasDroneMode)
        {
            mWasDroneMode = false;
            mIsNoteOn = false;
            // mEnvelope.setState(EnvelopeState::Release);
        }
        
        for (const auto metadata : midiMessages)
        {
            processMidiMessage(metadata.getMessage());
        }
    }
    else if (!mWasDroneMode)
    {
        mWasDroneMode = true;
        // mEnvelope.setState(EnvelopeState::Attack);
    }
    
    mWaveTableOsc.setFrequency(mFrequency, static_cast<int>(mSampleRate));
    
    const int numSamples = buffer.getNumSamples();
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float envelopeLevel = mEnvelope.updateEnvelope();
        float outputSample = 0.0f;
        
        outputSample = mWaveTableOsc.getNextSample(mXPosition, mYPosition);
        outputSample *= envelopeLevel;
        
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, sample, outputSample);
        }
    }
}

void FromFileToWaveAudioProcessor::processMidiMessage(const juce::MidiMessage& message)
{
    if (message.isNoteOn())
    {
        mCurrentNote = message.getNoteNumber();
        mFrequency = noteToFrequency(mCurrentNote);
        mIsNoteOn = true;
        mEnvelope.setState(EnvelopeState::Attack);
    }
    else if (message.isNoteOff())
    {
        if (message.getNoteNumber() == mCurrentNote)
        {
            mIsNoteOn = false;
            mCurrentNote = -1;
            mEnvelope.setState(EnvelopeState::Release);
        }
    }
}

float FromFileToWaveAudioProcessor::noteToFrequency(int midiNote) const
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.f) / 12.0f);
}

bool FromFileToWaveAudioProcessor::loadWaveTableFile(int slot, const juce::File& file, const WaveTableFileReader::Config& config)
{
    return mWaveTableOsc.loadWaveTableFile(slot, file, config);
}

juce::String FromFileToWaveAudioProcessor::getSlotError(int slot) const
{
    return mWaveTableOsc.getSlotError(slot);
}

bool FromFileToWaveAudioProcessor::isSlotLoaded(int slot) const
{
    return mWaveTableOsc.isSlotLoaded(slot);
}

//==============================================================================
bool FromFileToWaveAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FromFileToWaveAudioProcessor::createEditor()
{
    return new FromFileToWaveAudioProcessorEditor (*this);
}

//==============================================================================
void FromFileToWaveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::XmlElement xml("FromFileToWaveSettings");
    
    xml.setAttribute("xPosition", mXPosition);
    xml.setAttribute("yPosition", mYPosition);
    xml.setAttribute("frequency", mFrequency);
    xml.setAttribute("droneMode", mDroneMode);
    xml.setAttribute("attackTimeMs", static_cast<double>(mEnvelope.getAttack()));
    xml.setAttribute("releaseTimeMs", static_cast<double>(mEnvelope.getRelease()));
    
    for (int i = 0; i < NumWaveTableSlots; ++i)
    {
        xml.setAttribute("bitDepth" + juce::String(i), mBitDepth[static_cast<size_t>(i)]);
        xml.setAttribute("tableSize" + juce::String(i), mTableSize[static_cast<size_t>(i)]);
        xml.setAttribute("numTables" + juce::String(i), mNumTables[static_cast<size_t>(i)]);
        xml.setAttribute("smooth" + juce::String(i), static_cast<double>(mSmooth[static_cast<size_t>(i)]));
    }
    
    copyXmlToBinary(xml, destData);
}

void FromFileToWaveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml != nullptr)
    {
        if (xml->hasTagName("FromFileToWaveSettings"))
        {
            mXPosition = static_cast<float>(xml->getDoubleAttribute("xPosition", 0.0));
            mYPosition = static_cast<float>(xml->getDoubleAttribute("yPosition", 0.0));
            mFrequency = static_cast<float>(xml->getDoubleAttribute("frequency", 440.0));
            mDroneMode = xml->getBoolAttribute("droneMode", true);
            mEnvelope.setAttack(static_cast<float>(xml->getDoubleAttribute("attackTimeMs", 2.0)));
            mEnvelope.setRelease(static_cast<float>(xml->getDoubleAttribute("releaseTimeMs", 50.0)));
            
            mEnvelope.updateEnvelopeIncrements(static_cast<float>(mSampleRate));
            
            for (int i = 0; i < NumWaveTableSlots; ++i)
            {
                mBitDepth[static_cast<size_t>(i)] = xml->getIntAttribute("bitDepth" + juce::String(i), 16);
                mTableSize[static_cast<size_t>(i)] = xml->getIntAttribute("tableSize" + juce::String(i), 2048);
                mNumTables[static_cast<size_t>(i)] = xml->getIntAttribute("numTables" + juce::String(i), 1);
                mSmooth[static_cast<size_t>(i)] = static_cast<float>(xml->getDoubleAttribute("smooth" + juce::String(i), 0.0));
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FromFileToWaveAudioProcessor();
}
