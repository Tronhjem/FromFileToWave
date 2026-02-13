/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;
//==============================================================================
FromFileToWaveAudioProcessorEditor::FromFileToWaveAudioProcessorEditor (FromFileToWaveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(800, 600);

    mBitDepthLabel.setText("Bit Depth:", juce::dontSendNotification);
    mBitDepthLabel.setJustificationType(juce::Justification::centredRight);
    
    mBitDepthCombo.addItem("8-bit", 1);
    mBitDepthCombo.addItem("16-bit", 2);
    mBitDepthCombo.addItem("24-bit", 3);
    mBitDepthCombo.addItem("32-bit", 4);
    mBitDepthCombo.setSelectedId(2);
    
    mTableSizeLabel.setText("Table Size:", juce::dontSendNotification);
    mTableSizeLabel.setJustificationType(juce::Justification::centredRight);
    
    mTableSizeCombo.addItem("512", 1);
    mTableSizeCombo.addItem("1024", 2);
    mTableSizeCombo.addItem("2048", 3);
    mTableSizeCombo.addItem("4096", 4);
    mTableSizeCombo.addItem("8192", 5);
    mTableSizeCombo.setSelectedId(3);
    
    mNumTablesLabel.setText("Num Tables:", juce::dontSendNotification);
    mNumTablesLabel.setJustificationType(juce::Justification::centredRight);
    
    mNumTablesEditor.setText("1");
    mNumTablesEditor.setInputRestrictions(3, "0123456789");
    
    mLoadFileButton.setButtonText("Load Wavetable File");
    mReloadButton.setButtonText("Reload");

    mStatusLabel.setText("No file loaded", juce::dontSendNotification);
    mStatusLabel.setJustificationType(juce::Justification::centred);
    mStatusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    
    mFrequencyLabel.setText("Frequency (Hz):", juce::dontSendNotification);
    mFrequencyLabel.setJustificationType(juce::Justification::centredRight);
    
    mFrequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mFrequencySlider.setRange(2.0, 2000.0, 1.0);
    mFrequencySlider.setValue(audioProcessor.mFrequency);
    mFrequencySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    
    mWaveScanLabel.setText("Wave Scan:", juce::dontSendNotification);
    mWaveScanLabel.setJustificationType(juce::Justification::centredRight);

    mFileScanSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mFileScanSlider.setRange(0.0, 1.0, 0.0);
    mFileScanSlider.setValue(audioProcessor.mWaveScan);
    mFileScanSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    
    mBitDepthCombo.addListener(this);
    mTableSizeCombo.addListener(this);
    mLoadFileButton.addListener(this);
    mReloadButton.addListener(this);
    mFrequencySlider.addListener(this);
    mFileScanSlider.addListener(this);
    
    addAndMakeVisible(mBitDepthLabel);
    addAndMakeVisible(mBitDepthCombo);
    addAndMakeVisible(mTableSizeLabel);
    addAndMakeVisible(mNumTablesEditor);
    addAndMakeVisible(mTableSizeCombo);
    addAndMakeVisible(mNumTablesLabel);
    addAndMakeVisible(mLoadFileButton);
    addAndMakeVisible(mReloadButton);
    addAndMakeVisible(mStatusLabel);
    addAndMakeVisible(mFrequencyLabel);
    addAndMakeVisible(mFrequencySlider);
    addAndMakeVisible(mWaveScanLabel);
    addAndMakeVisible(mFileScanSlider);
}


FromFileToWaveAudioProcessorEditor::~FromFileToWaveAudioProcessorEditor()
{
}

void FromFileToWaveAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &mFileScanSlider)
    {
        audioProcessor.mWaveScan = static_cast<float>(slider->getValue());
    }
    else if (slider == &mFrequencySlider)
    {
        audioProcessor.mFrequency = static_cast<float>(slider->getValue());
    }
}

void FromFileToWaveAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &mBitDepthCombo)
    {
        int selectedId = comboBox->getSelectedId();
        switch (selectedId)
        {
            case 1: audioProcessor.mBitDepth = 8; break;
            case 2: audioProcessor.mBitDepth = 16; break;
            case 3: audioProcessor.mBitDepth = 24; break;
            case 4: audioProcessor.mBitDepth = 32; break;
            default: audioProcessor.mBitDepth = 16; break;
        }
    }
    else if (comboBox == &mTableSizeCombo)
    {
        int selectedId = comboBox->getSelectedId();
        switch (selectedId)
        {
            case 1: audioProcessor.mTableSize = 512; break;
            case 2: audioProcessor.mTableSize = 1024; break;
            case 3: audioProcessor.mTableSize = 2048; break;
            case 4: audioProcessor.mTableSize = 4096; break;
            case 5: audioProcessor.mTableSize = 8192; break;
            default: audioProcessor.mTableSize = 2048; break;
        }
    }
}

void FromFileToWaveAudioProcessorEditor::loadFile(juce::File file)
{
   WaveTableFileReader::BitDepth bitDepth = static_cast<WaveTableFileReader::BitDepth>(audioProcessor.mBitDepth);
    WaveTableFileReader::Config config{bitDepth, audioProcessor.mTableSize, audioProcessor.mNumTables};

    bool success = audioProcessor.loadWaveTableFile(file, config);
    mFile = file;

    if (success)
    {
        mStatusLabel.setText("Loaded: " + file.getFileName(), juce::dontSendNotification);
        mStatusLabel.setColour(juce::Label::textColourId, juce::Colours::green);
    }
    else
    {
        juce::String error = audioProcessor.getLastLoadError();
        mStatusLabel.setText("Error: " + error, juce::dontSendNotification);
        mStatusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    }
}

void FromFileToWaveAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &mLoadFileButton)
    {
        int numTables = mNumTablesEditor.getText().getIntValue();

        if (numTables < 1 || numTables > 999)
        {
            mStatusLabel.setText("Error: Number of tables must be 1-999", juce::dontSendNotification);
            mStatusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
            return;
        }

        audioProcessor.mNumTables = numTables;
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        
        mFileChooser = std::make_unique<juce::FileChooser>("Select a wavetable file...",
                                                             juce::File());
        
        mFileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser)
        {
            juce::File file = chooser.getResult();
            
            if (file == juce::File{})
                return;

            loadFile(file);
        });
    }
    
    else if (button == &mReloadButton)
    {
        int numTables = mNumTablesEditor.getText().getIntValue();

        if (numTables < 1 || numTables > 999)
        {
            mStatusLabel.setText("Error: Number of tables must be 1-999", juce::dontSendNotification);
            mStatusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
            return;
        }
        
        audioProcessor.mNumTables = numTables;
        loadFile(mFile);
    }
}

//==============================================================================
void FromFileToWaveAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
}

void FromFileToWaveAudioProcessorEditor::resized()
{
    constexpr int rowHeight = 30;
    constexpr int rowMargin = 10;
    Rectangle bounds = getLocalBounds().reduced(20);
    
    Rectangle fileSection = bounds.removeFromTop(200);
    
    Rectangle row1 = fileSection.removeFromTop(rowHeight);
    mBitDepthLabel.setBounds(row1.removeFromLeft(100));
    mBitDepthCombo.setBounds(row1.removeFromLeft(120));
    
    fileSection.removeFromTop(rowMargin);
    Rectangle row2 = fileSection.removeFromTop(rowHeight);
    mTableSizeLabel.setBounds(row2.removeFromLeft(100));
    mTableSizeCombo.setBounds(row2.removeFromLeft(120));
    
    fileSection.removeFromTop(rowMargin);
    Rectangle row3 = fileSection.removeFromTop(rowHeight);
    mNumTablesLabel.setBounds(row3.removeFromLeft(100));
    mNumTablesEditor.setBounds(row3.removeFromLeft(120));
    
    fileSection.removeFromTop(rowMargin);
    Rectangle row4 = fileSection.removeFromTop(rowHeight);
    mLoadFileButton.setBounds(row4.removeFromLeft(220).reduced(0, 2));
    
    fileSection.removeFromTop(rowMargin);
    Rectangle row5 = fileSection.removeFromTop(rowHeight);
    mReloadButton.setBounds(row5.removeFromLeft(220).reduced(0, 2));

    bounds.removeFromTop(rowMargin);
    mStatusLabel.setBounds(bounds.removeFromTop(25));
    
    bounds.removeFromTop(20);
    Rectangle freqRow = bounds.removeFromTop(rowHeight);
    mFrequencyLabel.setBounds(freqRow.removeFromLeft(100));
    mFrequencySlider.setBounds(freqRow);
    
    bounds.removeFromTop(rowMargin);
    Rectangle scanRow = bounds.removeFromTop(rowHeight);
    mWaveScanLabel.setBounds(scanRow.removeFromLeft(100));
    mFileScanSlider.setBounds(scanRow);
}
