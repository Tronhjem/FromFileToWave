
#include "WaveTableOsc.h"

WaveTableOsc::WaveTableOsc(float freq, int sampleRate)
    : mFrequency(freq)
    , mSampleRate(sampleRate)
{
}

float WaveTableOsc::lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float WaveTableOsc::getNextSample(float xIndex, float yIndex)
{
    yIndex = std::clamp(yIndex, 0.0f, 1.0f);
    
    const float yScaled = yIndex * (NumWaveTableSlots - 1);
    const int yFloor = static_cast<int>(std::floor(yScaled));
    const int yUpper = std::min(yFloor + 1, NumWaveTableSlots - 1);
    const float yFrac = yScaled - static_cast<float>(yFloor);
    
    float sampleLower = 0.0f;
    float sampleUpper = 0.0f;
    
    if (mOscillators[yFloor].isWaveTableLoaded())
    {
        sampleLower = mOscillators[yFloor].getSample(mFrequency, mSampleRate, xIndex);
    }
    
    if (mOscillators[yUpper].isWaveTableLoaded())
    {
        sampleUpper = mOscillators[yUpper].getSample(mFrequency, mSampleRate, xIndex);
    }

    return lerp(sampleLower, sampleUpper, yFrac);
}

void WaveTableOsc::setFrequency(float freq, int sampleRate)
{
    mFrequency = freq;
    mSampleRate = sampleRate;
}

bool WaveTableOsc::loadWaveTableFile(int slot, const juce::File& file, const WaveTableFileReader::Config& config)
{
    if (slot < 0 || slot >= NumWaveTableSlots)
        return false;
    
    return mOscillators[slot].loadWaveTable(file, config);
}

bool WaveTableOsc::isSlotLoaded(int slot) const
{
    if (slot < 0 || slot >= NumWaveTableSlots)
        return false;
    
    return mOscillators[slot].isWaveTableLoaded();
}

juce::String WaveTableOsc::getSlotError(int slot) const
{
    if (slot < 0 || slot >= NumWaveTableSlots)
        return "Invalid slot index";
    
    return mOscillators[slot].getLastError();
}
