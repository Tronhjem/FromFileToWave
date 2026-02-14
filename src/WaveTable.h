#pragma once

#include "WaveTableFileReader.h"

constexpr double twoPi = 6.283185307179586;

class WaveTable
{
public:
    WaveTable() = default;
    ~WaveTable() = default;

    float getSample(float delta, float xIndex);
    
    bool loadWaveTable(const juce::File& file, const WaveTableFileReader::Config& config);
    bool isWaveTableLoaded() const { return mWaveReader.isLoaded(); }
    juce::String getLastError() const { return mWaveReader.getLastError(); }

private:
    inline float lerp(float a, float b, float t);

    WaveTableFileReader mWaveReader;
};
