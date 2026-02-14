#pragma once

#include "WaveTableFileReader.h"

constexpr double twoPi = 6.283185307179586;

class WaveTable
{
public:
    WaveTable();
    ~WaveTable() = default;

    float getSample(float frequency, int sampleRate, float xIndex);
    
    bool loadWaveTable(const juce::File& file, const WaveTableFileReader::Config& config);
    bool isWaveTableLoaded() const { return mWaveReader.isLoaded(); }
    juce::String getLastError() const { return mWaveReader.getLastError(); }

private:
    inline float lerp(float a, float b, float t);
    float mDelta;
    
    WaveTableFileReader mWaveReader;
};
