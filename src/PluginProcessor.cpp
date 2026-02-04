/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WaveTableFileReader.h"

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
                       ), mOsc(700, 44100, mWaveTableReader)
#endif
{
    juce::File file {"/Users/christiantronhjem/dev/JuceProjects/FromFileToWave/data/testFile.txt"};
    WaveTableFileReader::Config conf {WaveTableFileReader::BitDepth::Bit16, 1024, 3};
    mWaveTableReader.loadFile(file, conf);
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
}

const juce::String FromFileToWaveAudioProcessor::getProgramName (int index)
{
    return {};
}

void FromFileToWaveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FromFileToWaveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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

    const std::vector<std::vector<double>>& table = mWaveTableReader.getWaveTables();
    for (int channel = 0; channel < 1; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        const int numSamples = buffer.getNumSamples();
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = mOsc.getNextSample(0);
        }
    }
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FromFileToWaveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
bool FromFileToWaveAudioProcessor::loadWaveTableFromFile(
    const juce::File& file,
    WaveTableFileReader::BitDepth bitDepth,
    int tableSize,
    int numTables)
{
    WaveTableFileReader::Config config;
    config.bitDepth = bitDepth;
    config.tableSize = tableSize;
    config.numTables = numTables;
    
    bool success = mWaveTableReader.loadFile(file, config);
    
    if (success)
    {
        DBG("Successfully loaded wave tables from: " << file.getFullPathName());
        DBG("Tables: " << numTables << ", Table size: " << tableSize << ", Bit depth: " << (int)bitDepth);
    }
    else
    {
        DBG("Failed to load wave tables: " << waveTableReader.getLastError());
    }
    
    return success;
}

const std::vector<std::vector<double>>& FromFileToWaveAudioProcessor::getWaveTables() const
{
    return mWaveTableReader.getWaveTables();
}

bool FromFileToWaveAudioProcessor::hasWaveTablesLoaded() const
{
    return mWaveTableReader.isLoaded();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FromFileToWaveAudioProcessor();
}
