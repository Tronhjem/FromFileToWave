/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WaveTableOsc.h"

//==============================================================================
/**
*/
class FromFileToWaveAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FromFileToWaveAudioProcessor();
    ~FromFileToWaveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    bool loadWaveTableFile(int slot, const juce::File& file, const WaveTableFileReader::Config& config);
    juce::String getSlotError(int slot) const;
    bool isSlotLoaded(int slot) const;
    
    float mXPosition = 0.f;
    float mYPosition = 0.f;
    float mFrequency = 440.f;
    bool mDroneMode = true;
    bool mIsNoteOn = false;
    
    std::array<int, NumWaveTableSlots> mBitDepth = {16, 16, 16, 16, 16, 16};
    std::array<int, NumWaveTableSlots> mTableSize = {2048, 2048, 2048, 2048, 2048, 2048};
    std::array<int, NumWaveTableSlots> mNumTables = {1, 1, 1, 1, 1, 1};
    std::array<float, NumWaveTableSlots> mSmooth = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

private:
    double mSampleRate;
    bool mWasDroneMode;
    WaveTableOsc mWaveTableOsc;
    
    int mCurrentNote = -1;
    
    void processMidiMessage(const juce::MidiMessage& message);
    float noteToFrequency(int midiNote) const;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FromFileToWaveAudioProcessor)
};
