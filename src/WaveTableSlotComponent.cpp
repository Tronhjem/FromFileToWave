#include "WaveTableSlotComponent.h"

WaveTableSlotComponent::WaveTableSlotComponent(int slotIndex, float yPosition)
    : mSlotIndex(slotIndex)
    , mYPosition(yPosition)
{
    mBitDepthLabel.setText("Bit:", juce::dontSendNotification);
    mBitDepthLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(mBitDepthLabel);
    
    mBitDepthCombo.addItem("8", 1);
    mBitDepthCombo.addItem("16", 2);
    mBitDepthCombo.addItem("24", 3);
    mBitDepthCombo.addItem("32", 4);
    mBitDepthCombo.setSelectedId(2);
    mBitDepthCombo.onChange = [this]() { 
        handleBitDepthChange(); 
        tryReload();
    };
    addAndMakeVisible(mBitDepthCombo);
    
    mTableSizeLabel.setText("Size:", juce::dontSendNotification);
    mTableSizeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(mTableSizeLabel);
    
    mTableSizeCombo.addItem("512", 1);
    mTableSizeCombo.addItem("1024", 2);
    mTableSizeCombo.addItem("2048", 3);
    mTableSizeCombo.addItem("4096", 4);
    mTableSizeCombo.addItem("8192", 5);
    mTableSizeCombo.setSelectedId(3);
    mTableSizeCombo.onChange = [this]() { 
        handleTableSizeChange(); 
        tryReload();
    };
    addAndMakeVisible(mTableSizeCombo);
    
    mNumTablesLabel.setText("Num:", juce::dontSendNotification);
    mNumTablesLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(mNumTablesLabel);
    
    mNumTablesEditor.setText("1");
    mNumTablesEditor.setInputRestrictions(3, "0123456789");
    mNumTablesEditor.onTextChange = [this]() {
        handleNumTablesChange();
        tryReload();
    };
    addAndMakeVisible(mNumTablesEditor);
    
    mSmoothLabel.setText("Sm:", juce::dontSendNotification);
    mSmoothLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(mSmoothLabel);
    
    mSmoothSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    mSmoothSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    mSmoothSlider.setRange(0.0, 1.0, 0.01);
    mSmoothSlider.setValue(0.0);
    mSmoothSlider.onValueChange = [this]() {
        handleSmoothChange();
        tryReload();
    };
    addAndMakeVisible(mSmoothSlider);
    
    mStatusLabel.setText("Drop file here", juce::dontSendNotification);
    mStatusLabel.setJustificationType(juce::Justification::centred);
    mStatusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(mStatusLabel);
}

void WaveTableSlotComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.setColour(juce::Colours::darkgrey.darker());
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(bounds.toFloat().reduced(1), 4.0f, 1.0f);
}

void WaveTableSlotComponent::resized()
{
    constexpr int rowHeight = 25;
    constexpr int labelWidth = 40;
    constexpr int comboWidth = 80;
    constexpr int margin = 2;
    
    auto bounds = getLocalBounds().reduced(5);
    
    auto configRow = bounds.removeFromTop(rowHeight);
    mBitDepthLabel.setBounds(configRow.removeFromLeft(labelWidth));
    mBitDepthCombo.setBounds(configRow.removeFromLeft(comboWidth));
    configRow.removeFromLeft(5);
    mTableSizeLabel.setBounds(configRow.removeFromLeft(labelWidth));
    mTableSizeCombo.setBounds(configRow.removeFromLeft(comboWidth));
    configRow.removeFromLeft(5);
    mNumTablesLabel.setBounds(configRow.removeFromLeft(labelWidth));
    mNumTablesEditor.setBounds(configRow.removeFromLeft(comboWidth));
    configRow.removeFromLeft(5);
    mSmoothLabel.setBounds(configRow.removeFromLeft(labelWidth));
    mSmoothSlider.setBounds(configRow.removeFromLeft(50));
    
    bounds.removeFromTop(margin);
    
    mStatusLabel.setBounds(bounds.removeFromTop(rowHeight));
}

bool WaveTableSlotComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    return files.size() > 0;
}

void WaveTableSlotComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    if (files.isEmpty())
        return;
    
    juce::File file(files[0]);
    loadFile(file);
}

void WaveTableSlotComponent::loadFile(const juce::File& file)
{
    int numTables = mNumTablesEditor.getText().getIntValue();
    
    if (numTables < 1 || numTables > 999)
    {
        setStatus("Error: Num tables must be 1-999", juce::Colours::red);
        return;
    }
    
    mConfig.numTables = numTables;
    mLoadedFile = file;
    
    if (onFileLoaded)
    {
        onFileLoaded(mSlotIndex, file, mConfig);
    }
}

void WaveTableSlotComponent::tryReload()
{
    if (mLoadedFile.exists())
    {
        loadFile(mLoadedFile);
    }
}

void WaveTableSlotComponent::handleBitDepthChange()
{
    int selectedId = mBitDepthCombo.getSelectedId();
    switch (selectedId)
    {
        case 1: mConfig.bitDepth = 8; break;
        case 2: mConfig.bitDepth = 16; break;
        case 3: mConfig.bitDepth = 24; break;
        case 4: mConfig.bitDepth = 32; break;
        default: mConfig.bitDepth = 16; break;
    }
}

void WaveTableSlotComponent::handleTableSizeChange()
{
    int selectedId = mTableSizeCombo.getSelectedId();
    switch (selectedId)
    {
        case 1: mConfig.tableSize = 512; break;
        case 2: mConfig.tableSize = 1024; break;
        case 3: mConfig.tableSize = 2048; break;
        case 4: mConfig.tableSize = 4096; break;
        case 5: mConfig.tableSize = 8192; break;
        default: mConfig.tableSize = 2048; break;
    }
}

void WaveTableSlotComponent::handleNumTablesChange()
{
    int numTables = mNumTablesEditor.getText().getIntValue();
    if (numTables >= 1 && numTables <= 999)
    {
        mConfig.numTables = numTables;
    }
}

void WaveTableSlotComponent::handleSmoothChange()
{
    mConfig.smoothAmount = static_cast<float>(mSmoothSlider.getValue());
}

WaveTableSlotComponent::Config WaveTableSlotComponent::getConfig() const
{
    return mConfig;
}

void WaveTableSlotComponent::setConfig(const Config& config)
{
    mConfig = config;
    
    switch (config.bitDepth)
    {
        case 8: mBitDepthCombo.setSelectedId(1); break;
        case 16: mBitDepthCombo.setSelectedId(2); break;
        case 24: mBitDepthCombo.setSelectedId(3); break;
        case 32: mBitDepthCombo.setSelectedId(4); break;
        default: mBitDepthCombo.setSelectedId(2); break;
    }
    
    switch (config.tableSize)
    {
        case 512: mTableSizeCombo.setSelectedId(1); break;
        case 1024: mTableSizeCombo.setSelectedId(2); break;
        case 2048: mTableSizeCombo.setSelectedId(3); break;
        case 4096: mTableSizeCombo.setSelectedId(4); break;
        case 8192: mTableSizeCombo.setSelectedId(5); break;
        default: mTableSizeCombo.setSelectedId(3); break;
    }
    
    mNumTablesEditor.setText(juce::String(config.numTables));
    mSmoothSlider.setValue(config.smoothAmount, juce::dontSendNotification);
}

void WaveTableSlotComponent::setStatus(const juce::String& message, juce::Colour colour)
{
    mStatusLabel.setText(message, juce::dontSendNotification);
    mStatusLabel.setColour(juce::Label::textColourId, colour);
}

void WaveTableSlotComponent::setLoadedFile(const juce::File& file)
{
    mLoadedFile = file;
}
