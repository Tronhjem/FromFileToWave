#pragma once

#include <JuceHeader.h>
#include <vector>
#include <atomic>

class WaveTableFileReader
{
public:
    enum class BitDepth
    {
        Bit8 = 8,
        Bit16 = 16,
        Bit24 = 24,
        Bit32 = 32
    };
    
    struct Config
    {
        BitDepth bitDepth = BitDepth::Bit16;
        int tableSize = 2048;
        int numTables = 1;
        float smoothAmount = 0.0f;
    };
    
    WaveTableFileReader() = default;
    ~WaveTableFileReader() = default;
    
    bool loadFile(const juce::File& file, const Config& config);
    const Config& getConfig() const { return mLastConfig; }
    
    //TODO: Not thread safe, implement a pointer swap and lock.
    const std::vector<std::vector<float>>& getWaveTables() const { return mWaveTables; }
    
    bool isLoaded() const { return mIsLoaded.load(); }
    
    juce::String getLastError() const { return mLastError; }
    
private:
    struct WavHeader
    {
        juce::uint16 audioFormat;
        juce::uint16 numChannels;
        juce::uint32 sampleRate;
        juce::uint16 bitsPerSample;
        juce::uint32 dataChunkSize;
        juce::int64 dataChunkOffset;
    };
    
    std::vector<std::vector<float>> mWaveTables;
    juce::String mLastError;
    Config mLastConfig;
    
    float convertSampleToFloat(const juce::uint8* bytes, BitDepth bitDepth, bool isIEEEFloat = false);
    int getBytesPerSample(BitDepth bitDepth) const;
    
    bool isWavFile(juce::FileInputStream& stream);
    bool parseWavHeader(juce::FileInputStream& stream, WavHeader& header);
    bool loadWavFile(juce::FileInputStream& stream, const WavHeader& header, const Config& config);
    bool loadRawFile(juce::FileInputStream& stream, const Config& config);
    void applyLowpassFilter(std::vector<float>& table, float smoothAmount);
    std::atomic<bool> mIsLoaded = false;
};
