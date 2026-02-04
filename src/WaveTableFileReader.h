#pragma once

#include <JuceHeader.h>
#include <vector>

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
    };
    
    WaveTableFileReader() = default;
    ~WaveTableFileReader() = default;
    
    bool loadFile(const juce::File& file, const Config& config);
    
    const std::vector<std::vector<double>>& getWaveTables() const { return waveTables; }
    
    bool isLoaded() const { return !waveTables.empty(); }
    
    juce::String getLastError() const { return lastError; }
    
private:
    std::vector<std::vector<double>> waveTables;
    juce::String lastError;
    
    double convertSampleToDouble(const uint8* bytes, BitDepth bitDepth);
    int getBytesPerSample(BitDepth bitDepth) const;
};
