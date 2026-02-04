
#include <cmath>
#include "WaveTableOsc.h"


float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

double WaveTableOsc::getNextSample(const float index)
{
    const float delta = (mFrequency / (float)mSampleRate) * (float)mTestOsc.size();

    int deltaFloor = static_cast<int>(floor(mDelta));
    int deltaUpper = deltaFloor + 1;
    if (deltaUpper >= mTestOsc.size())
        deltaUpper = 0;

    float a = mTestOsc[deltaFloor];
    float b = mTestOsc[deltaUpper];

    float sample = lerp(a, b, mDelta - deltaFloor); 
    mDelta += delta;
    if (mDelta > mTestOsc.size())
        mDelta = 0;
    return sample;
}
