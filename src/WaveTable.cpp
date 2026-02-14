
#include <cmath>
#include <cassert>
#include "WaveTable.h"
#include "WaveTableFileReader.h"

bool WaveTable::loadWaveTable(const juce::File& file, const WaveTableFileReader::Config& config)
{
    return mWaveReader.loadFile(file, config);
}

float WaveTable::lerp(float a, float b, float t) 
{
    return (1 - t) * a + t * b;
}

float WaveTable::getSample(float delta, float xIndex)
{
    if (!mWaveReader.isLoaded())
        return 0.f;

    const std::vector<std::vector<float>>& tables = mWaveReader.getWaveTables();

    WaveTableFileReader::Config waveConfig = mWaveReader.getConfig();
    const int maxTableIndex = waveConfig.numTables - 1;
    const int tableSizeMask = waveConfig.tableSize - 1;
    
#if _DEBUG
    assert((tableSize > 0) && ((tableSize & (tableSize - 1)) == 0) && "n must be a power of two");
#endif

    const float tableIndex = xIndex * static_cast<float>(maxTableIndex);
    const int tableFloorIndex = static_cast<int>(floor(tableIndex));
    const int tableUpperIndex = std::min(tableFloorIndex + 1, maxTableIndex);

    const int deltaFloor = static_cast<int>(floor(delta));
    const int deltaUpper = (deltaFloor + 1) & tableSizeMask;
    
    const float floorA = tables[static_cast<size_t>(tableFloorIndex)][static_cast<size_t>(deltaFloor)];
    const float floorB = tables[static_cast<size_t>(tableFloorIndex)][static_cast<size_t>(deltaUpper)];
    const float floorSample = lerp(floorA, floorB, delta - static_cast<float>(deltaFloor)); 

    const float upperA = tables[static_cast<size_t>(tableUpperIndex)][static_cast<size_t>(deltaFloor)];
    const float upperB = tables[static_cast<size_t>(tableUpperIndex)][static_cast<size_t>(deltaUpper)];
    const float upperSample = lerp(upperA, upperB, delta - static_cast<float>(deltaFloor)); 

    const float sample = lerp(floorSample, upperSample, tableIndex - static_cast<float>(tableFloorIndex));

    return sample;
}
