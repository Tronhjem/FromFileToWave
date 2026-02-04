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
class FromFileToWaveAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    FromFileToWaveAudioProcessorEditor (FromFileToWaveAudioProcessor&);
    ~FromFileToWaveAudioProcessorEditor() override;
    void sliderValueChanged (juce::Slider* slider) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    FromFileToWaveAudioProcessor& audioProcessor;
    juce::Slider mFileScanSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FromFileToWaveAudioProcessorEditor)
};
