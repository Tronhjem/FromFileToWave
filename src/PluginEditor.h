#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "XYPadComponent.h"
#include "WaveTableSlotComponent.h"

//==============================================================================
/**
*/
class FromFileToWaveAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                              public juce::Slider::Listener
{
public:
    FromFileToWaveAudioProcessorEditor (FromFileToWaveAudioProcessor&);
    ~FromFileToWaveAudioProcessorEditor() override;
    void sliderValueChanged (juce::Slider* slider) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void handleFileLoaded(int slotIndex, juce::File file, WaveTableSlotComponent::Config config);
    
    FromFileToWaveAudioProcessor& audioProcessor;
    
    XYPadComponent mXYPad;
    juce::OwnedArray<WaveTableSlotComponent> mSlots;
    
    juce::Label mFrequencyLabel;
    juce::Slider mFrequencySlider;
    juce::ToggleButton mDroneModeButton;
    
    juce::Label mAttackLabel;
    juce::Slider mAttackSlider;
    juce::Label mReleaseLabel;
    juce::Slider mReleaseSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FromFileToWaveAudioProcessorEditor)
};
