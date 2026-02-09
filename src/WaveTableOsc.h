
#pragma once

#include "WaveTableFileReader.h"

// constexpr int TableSize = 1024;
// constexpr int HalfTableSize = TableSize / 2;
// constexpr int QuaterTableSize = TableSize / 4;
// constexpr int NumOfTables = 3; 
// constexpr int WaveTableSize = TableSize * NumOfTables;
constexpr double twoPi = 6.283185307179586;

class WaveTableOsc
{
public:
    WaveTableOsc(const float freq, const int sampleRate);
    ~WaveTableOsc() = default;

    float getNextSample(const float index);

    void setFrequency(const float freq, int sampleRate)
    {
        mFrequency = freq;
        mSampleRate = sampleRate;
    }

private:
    WaveTableOsc() = delete;

    inline float lerp(float a, float b, float t);

    float mFrequency;
    float mDelta;
    int mSampleRate;
    // std::array<float, WaveTableSize> mTestOsc;
    WaveTableFileReader mWaveReader;
};
