/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FromFileToWaveAudioProcessorEditor::FromFileToWaveAudioProcessorEditor (FromFileToWaveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 600);

    mFileScanSlider.setSliderStyle(Slider::LinearBarVertical);
    mFileScanSlider.setRange(0.0, 1.0, 0.0);
    mFileScanSlider.setBounds(20, 20, 60, 300);

    addAndMakeVisible(&mFileScanSlider);
}


FromFileToWaveAudioProcessorEditor::~FromFileToWaveAudioProcessorEditor()
{
}

void FromFileToWaveAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{

}

//==============================================================================
void FromFileToWaveAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);


}

void FromFileToWaveAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
