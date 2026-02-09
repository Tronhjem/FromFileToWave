/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"

//==============================================================================
/**
*/
class FromFileToWaveAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                              public juce::Slider::Listener,
                                              public juce::ComboBox::Listener,
                                              public juce::Button::Listener
{
public:
    FromFileToWaveAudioProcessorEditor (FromFileToWaveAudioProcessor&);
    ~FromFileToWaveAudioProcessorEditor() override;
    void sliderValueChanged (juce::Slider* slider) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;
    void buttonClicked (juce::Button* button) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    FromFileToWaveAudioProcessor& audioProcessor;
    
    juce::File mFile;
    
    juce::Label mBitDepthLabel;
    juce::ComboBox mBitDepthCombo;
    juce::Label mTableSizeLabel;
    juce::ComboBox mTableSizeCombo;
    juce::Label mNumTablesLabel;
    juce::TextEditor mNumTablesEditor;
    juce::TextButton mLoadFileButton;
    juce::TextButton mReloadButton;
    juce::Label mStatusLabel;
    
    juce::Label mFrequencyLabel;
    juce::Slider mFrequencySlider;
    juce::Label mWaveScanLabel;
    juce::Slider mFileScanSlider;
    
    std::unique_ptr<juce::FileChooser> mFileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FromFileToWaveAudioProcessorEditor)
};
