
#include <cmath>
#include <cassert>
#include "WaveTableOsc.h"
#include "WaveTableFileReader.h"

WaveTableOsc::WaveTableOsc(const float freq, const int sampleRate) 
            : mFrequency(freq), mDelta(0), mSampleRate(sampleRate), mWaveReader()
{
}

bool WaveTableOsc::loadWaveTable(const juce::File& file, const WaveTableFileReader::Config& config)
{
    mDelta = 0;
    return mWaveReader.loadFile(file, config);
}

float WaveTableOsc::lerp(float a, float b, float t) 
{
    return (1 - t) * a + t * b;
}

float WaveTableOsc::equalPower(float a, float b, float t) 
{
    return static_cast<float>(sqrt(0.5 * (1 - t)) * a + sqrt(0.5 * t) * b);
}

float WaveTableOsc::getNextSample(const float index)
{
    if (!mWaveReader.isLoaded())
        return 0.f;

    const std::vector<std::vector<float>>& tables = mWaveReader.getWaveTables();

    WaveTableFileReader::Config waveConfig = mWaveReader.getConfig();
    const int maxTableIndex = waveConfig.numTables - 1;
    const int tableSize = waveConfig.tableSize;
    const int tableSizeMask = waveConfig.tableSize - 1;
    
#if _DEBUG
    assert((tableSize > 0) && ((tableSize & (tableSize - 1)) == 0) && "n must be a power of two");
#endif

    const float tableIndex = index * static_cast<float>(maxTableIndex);
    const int tableFloorIndex = static_cast<int>(floor(tableIndex));
    const int tableUpperIndex = std::min(tableFloorIndex + 1, maxTableIndex);

    const float delta = (mFrequency / static_cast<float>(mSampleRate)) * static_cast<float>(tableSize);
    const int deltaFloor = static_cast<int>(floor(mDelta));
    const int deltaUpper = (deltaFloor + 1) & tableSizeMask;
    
    const float floorA = tables[static_cast<unsigned long>(tableFloorIndex)][static_cast<unsigned long>(deltaFloor)];
    const float floorB = tables[static_cast<unsigned long>(tableFloorIndex)][static_cast<unsigned long>(deltaUpper)];
    const float floorSample = lerp(floorA, floorB, mDelta - static_cast<float>(deltaFloor)); 

    const float upperA = tables[static_cast<unsigned long>(tableUpperIndex)][static_cast<unsigned long>(deltaFloor)];
    const float upperB = tables[static_cast<unsigned long>(tableUpperIndex)][static_cast<unsigned long>(deltaUpper)];
    const float upperSample = lerp(upperA, upperB, mDelta - static_cast<float>(deltaFloor)); 

    const float sample = lerp(floorSample, upperSample, tableIndex - static_cast<float>(tableFloorIndex));

    mDelta += delta;
    if (mDelta > static_cast<float>(tableSize))
        mDelta -= static_cast<float>(tableSize);

    return sample;
}
