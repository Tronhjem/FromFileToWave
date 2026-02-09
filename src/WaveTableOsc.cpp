
#include <cmath>
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

float WaveTableOsc::lerp(float v0, float v1, float t) 
{
    return (1 - t) * v0 + t * v1;
}

float WaveTableOsc::getNextSample(const float index)
{
    if (!mWaveReader.isLoaded())
        return 0.f;

    const std::vector<std::vector<float>>& tables = mWaveReader.getWaveTables();

    WaveTableFileReader::Config waveConfig = mWaveReader.getConfig();
    const int numOfTables = waveConfig.numTables;
    const int tableSize = waveConfig.tableSize;

    const float tableIndex = index * static_cast<float>(numOfTables - 1);
    const int tableFloorIndex = static_cast<int>(floor(index * static_cast<float>(numOfTables - 1)));
    int tableUpperIndex = tableFloorIndex + 1;
    if (tableUpperIndex >= numOfTables - 1)
        tableUpperIndex = numOfTables - 1;

    const float delta = (mFrequency / (float)mSampleRate) * (float)tableSize;
    const int deltaFloor = static_cast<int>(floor(mDelta));
    int deltaUpper = deltaFloor + 1;
    if (deltaUpper >= static_cast<int>(tableSize))
        deltaUpper = 0;
    
    const float floorA = tables[static_cast<unsigned long>(tableFloorIndex)][static_cast<unsigned long>(deltaFloor)];
    const float floorB = tables[static_cast<unsigned long>(tableFloorIndex)][static_cast<unsigned long>(deltaUpper)];
    const float floorSample = lerp(floorA, floorB, mDelta - static_cast<float>(deltaFloor)); 

    const float upperA = tables[static_cast<unsigned long>(tableUpperIndex)][static_cast<unsigned long>(deltaFloor)];
    const float upperB = tables[static_cast<unsigned long>(tableUpperIndex)][static_cast<unsigned long>(deltaUpper)];
    const float upperSample = lerp(upperA, upperB, mDelta - static_cast<float>(deltaFloor)); 

    const float sample = lerp(floorSample, upperSample, tableIndex - static_cast<float>(tableFloorIndex));

    mDelta += delta;
    if (mDelta > static_cast<float>(tableSize))
        mDelta = 0;

    return sample;
}
