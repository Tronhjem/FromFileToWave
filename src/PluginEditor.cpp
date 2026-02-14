/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "Envelope.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

constexpr float YPositions[NumWaveTableSlots] = {0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f};

//==============================================================================
FromFileToWaveAudioProcessorEditor::FromFileToWaveAudioProcessorEditor (FromFileToWaveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(1200, 700);

    addAndMakeVisible(mXYPad);
    mXYPad.setPosition(audioProcessor.mXPosition, audioProcessor.mYPosition);
    mXYPad.onPositionChanged = [this](float x, float y)
    {
        audioProcessor.mXPosition = x;
        audioProcessor.mYPosition = y;
    };
    
    mFrequencyLabel.setText("Frequency (Hz):", juce::dontSendNotification);
    mFrequencyLabel.setJustificationType(juce::Justification::centredLeft);
    mFrequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mFrequencySlider.setRange(2.0, 2000.0, 1.0);
    mFrequencySlider.setValue(audioProcessor.mFrequency);
    mFrequencySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    mFrequencySlider.addListener(this);
    addAndMakeVisible(mFrequencyLabel);
    addAndMakeVisible(mFrequencySlider);
    
    mDroneModeButton.setButtonText("Drone");
    mDroneModeButton.setToggleState(audioProcessor.mDroneMode, juce::dontSendNotification);
    mDroneModeButton.onClick = [this]()
    {
        audioProcessor.mDroneMode = mDroneModeButton.getToggleState();
        if (audioProcessor.mDroneMode)
        {
            audioProcessor.mIsNoteOn = true;
            audioProcessor.mEnvelope.setState(EnvelopeState::Attack);
        }
        else
        {
            audioProcessor.mEnvelope.setState(EnvelopeState::Release);
        }
    };
    addAndMakeVisible(mDroneModeButton);
    
    mAttackLabel.setText("A:", juce::dontSendNotification);
    mAttackLabel.setJustificationType(juce::Justification::centredLeft);
    mAttackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mAttackSlider.setRange(0.0, 2000.0, 0.1);
    mAttackSlider.setValue(audioProcessor.mEnvelope.getAttack());
    mAttackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    mAttackSlider.setTextValueSuffix("ms");
    mAttackSlider.addListener(this);
    addAndMakeVisible(mAttackLabel);
    addAndMakeVisible(mAttackSlider);
    
    mReleaseLabel.setText("R:", juce::dontSendNotification);
    mReleaseLabel.setJustificationType(juce::Justification::centredLeft);
    mReleaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mReleaseSlider.setRange(0.0, 2000.0, 0.1);
    mReleaseSlider.setValue(audioProcessor.mEnvelope.getRelease());
    mReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    mReleaseSlider.setTextValueSuffix("ms");
    mReleaseSlider.addListener(this);
    addAndMakeVisible(mReleaseLabel);
    addAndMakeVisible(mReleaseSlider);
    
    for (int i = 0; i < NumWaveTableSlots; ++i)
    {
        auto* slot = mSlots.add(new WaveTableSlotComponent(i, YPositions[i]));
        
        WaveTableSlotComponent::Config config;
        config.bitDepth = audioProcessor.mBitDepth[static_cast<size_t>(i)];
        config.tableSize = audioProcessor.mTableSize[static_cast<size_t>(i)];
        config.numTables = audioProcessor.mNumTables[static_cast<size_t>(i)];
        config.smoothAmount = audioProcessor.mSmooth[static_cast<size_t>(i)];
        slot->setConfig(config);
        
        slot->onFileLoaded = [this](const int slotIndex, juce::File file, WaveTableSlotComponent::Config cfg)
        {
            handleFileLoaded(slotIndex, file, cfg);
        };
        
        addAndMakeVisible(slot);
    }
    
    resized();
}

FromFileToWaveAudioProcessorEditor::~FromFileToWaveAudioProcessorEditor()
{
}

void FromFileToWaveAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &mFrequencySlider)
    {
        audioProcessor.mFrequency = static_cast<float>(slider->getValue());
    }
    else if (slider == &mAttackSlider)
    {
        audioProcessor.mEnvelope.setAttack(static_cast<float>(slider->getValue()));
    }
    else if (slider == &mReleaseSlider)
    {
        audioProcessor.mEnvelope.setRelease(static_cast<float>(slider->getValue()));
    }
}

void FromFileToWaveAudioProcessorEditor::handleFileLoaded(int slotIndex, juce::File file, WaveTableSlotComponent::Config config)
{
    audioProcessor.mBitDepth[static_cast<size_t>(slotIndex)] = config.bitDepth;
    audioProcessor.mTableSize[static_cast<size_t>(slotIndex)] = config.tableSize;
    audioProcessor.mNumTables[static_cast<size_t>(slotIndex)] = config.numTables;
    audioProcessor.mSmooth[static_cast<size_t>(slotIndex)] = config.smoothAmount;
    
    WaveTableFileReader::BitDepth bitDepth = static_cast<WaveTableFileReader::BitDepth>(config.bitDepth);
    WaveTableFileReader::Config readerConfig{bitDepth, config.tableSize, config.numTables, config.smoothAmount};
    
    bool success = audioProcessor.loadWaveTableFile(slotIndex, file, readerConfig);
    
    if (success)
    {
        mSlots[slotIndex]->setStatus("Loaded: " + file.getFileName(), juce::Colours::green);
        mSlots[slotIndex]->setLoadedFile(file);
    }
    else
    {
        juce::String error = audioProcessor.getSlotError(slotIndex);
        mSlots[slotIndex]->setStatus("Error: " + error, juce::Colours::red);
    }
}

//==============================================================================
void FromFileToWaveAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::red);
    g.drawText("Slots count: " + juce::String(mSlots.size()), 10, 10, 200, 20, juce::Justification::left);
}

void FromFileToWaveAudioProcessorEditor::resized()
{
    constexpr int margin = 10;
    constexpr int slotHeight = 100;
    constexpr int rowHeight = 25;
    
    auto bounds = getLocalBounds().reduced(margin);
    
    const int leftSectionWidth = bounds.getHeight() - 60;
    
    auto leftSection = bounds.removeFromLeft(leftSectionWidth);
    mXYPad.setBounds(leftSection.removeFromTop(leftSectionWidth));
    
    leftSection.removeFromTop(margin);
    auto freqRow = leftSection.removeFromTop(rowHeight);
    mFrequencyLabel.setBounds(freqRow.removeFromLeft(120));
    mFrequencySlider.setBounds(freqRow.removeFromLeft(freqRow.getWidth() - 80));
    freqRow.removeFromLeft(5);
    mDroneModeButton.setBounds(freqRow);
    
    leftSection.removeFromTop(margin);
    auto envRow = leftSection.removeFromTop(rowHeight);
    mAttackLabel.setBounds(envRow.removeFromLeft(20));
    mAttackSlider.setBounds(envRow.removeFromLeft(150));
    envRow.removeFromLeft(10);
    mReleaseLabel.setBounds(envRow.removeFromLeft(20));
    mReleaseSlider.setBounds(envRow.removeFromLeft(150));
    
    bounds.removeFromLeft(margin);
    
    for (int i = 0; i < NumWaveTableSlots; ++i)
    {
        if(mSlots[i])
        {
            auto slotBounds = bounds.removeFromTop(slotHeight);
            mSlots[i]->setBounds(slotBounds);
            bounds.removeFromTop(margin);
        }
    }
}
