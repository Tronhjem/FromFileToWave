#pragma once

#include <array>
#include "WaveTable.h"

constexpr int NumWaveTableSlots = 6;

class WaveTableOsc
{
public:
    WaveTableOsc(float freq, int sampleRate);
    ~WaveTableOsc() = default;

    float getNextSample(float xIndex, float yIndex);
    
    void setFrequency(float freq, int sampleRate);
    
    bool loadWaveTableFile(int slot, const juce::File& file, const WaveTableFileReader::Config& config);
    
    bool isSlotLoaded(int slot) const;
    juce::String getSlotError(int slot) const;

private:
    WaveTableOsc() = delete;
    
    inline float lerp(float a, float b, float t);
    
    float mFrequency;
    int mSampleRate;
    float mDelta;
    std::array<WaveTable, NumWaveTableSlots> mOscillators;
};
