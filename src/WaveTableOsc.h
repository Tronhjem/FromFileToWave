
#pragma once

#include <array>
#include "WaveTableFileReader.h"
class WaveTableOsc
{
public:
    WaveTableOsc(const float freq, const int sampleRate, const WaveTableFileReader& reader) 
            : mFrequency(freq), mSampleRate(sampleRate), mWaveReader(reader)
    { 
        mDelta = freq / static_cast<float>(sampleRate);


        // Fill test osc
        const double twoPi = 6.283185307179586;
        for (int i = 0; i < mTestOsc.size(); ++i)
        {
            mTestOsc[i] = (float)sin((twoPi * static_cast<double>(i)) / static_cast<float>(mTestOsc.size()));
        }
    }

    double getNextSample(const float index);

    void setFrequency(const float freq, int sampleRate)
    {
        mFrequency = freq;
        mSampleRate = sampleRate;
        mDelta = freq / static_cast<float>(sampleRate);
    }

private:
    float mFrequency;
    float mDelta;
    int mSampleRate;
    std::array<float, 1024> mTestOsc;
    const WaveTableFileReader& mWaveReader;
};
