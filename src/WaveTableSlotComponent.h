#pragma once

#include <JuceHeader.h>
#include <functional>

class WaveTableSlotComponent : public juce::Component,
                                public juce::FileDragAndDropTarget
{
public:
    struct Config
    {
        int bitDepth = 16;
        int tableSize = 2048;
        int numTables = 1;
        float smoothAmount = 0.0f;
    };
    
    WaveTableSlotComponent(int slotIndex, float yPosition);
    ~WaveTableSlotComponent() override = default;
    
    Config getConfig() const;
    void setConfig(const Config& config);
    
    void setStatus(const juce::String& message, juce::Colour colour);
    void setLoadedFile(const juce::File& file);
    juce::File getLoadedFile() const { return mLoadedFile; }
    
    std::function<void(int slotIndex, juce::File file, Config config)> onFileLoaded;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    void loadFile(const juce::File& file);
    void tryReload();
    void handleBitDepthChange();
    void handleTableSizeChange();
    void handleNumTablesChange();
    void handleSmoothChange();
    
    int mSlotIndex;
    float mYPosition;
    juce::File mLoadedFile;
    Config mConfig;
    
    juce::Label mBitDepthLabel;
    juce::ComboBox mBitDepthCombo;
    juce::Label mTableSizeLabel;
    juce::ComboBox mTableSizeCombo;
    juce::Label mNumTablesLabel;
    juce::TextEditor mNumTablesEditor;
    juce::Label mSmoothLabel;
    juce::Slider mSmoothSlider;
    juce::Label mStatusLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveTableSlotComponent)
};
