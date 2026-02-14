#include "Envelope.h"

void Envelope::updateEnvelopeIncrements(const float sampleRate)
{
    mSampleRate = sampleRate;

    if (mSampleRate <= 0) return;
    
    if (mAttackTimeMs > 0.0f)
        mAttackIncrement = 1.0f / (mAttackTimeMs * static_cast<float>(mSampleRate) / 1000.0f);
    else
        mAttackIncrement = 1.0f;
    
    if (mReleaseTimeMs > 0.0f)
        mReleaseIncrement = 1.0f / (mReleaseTimeMs * static_cast<float>(mSampleRate) / 1000.0f);
    else
        mReleaseIncrement = 1.0f;
}

void Envelope::setState(const EnvelopeState state)
{
    mEnvelopeState = state;
}

float Envelope::updateEnvelope()
{
    switch (mEnvelopeState)
    {
        case EnvelopeState::Attack:
            mEnvelopeLevel += mAttackIncrement;
            if (mEnvelopeLevel >= 1.0f)
            {
                mEnvelopeLevel = 1.0f;
                mEnvelopeState = EnvelopeState::Hold;
            }
            break;

        case EnvelopeState::Hold:
            break;
            
        case EnvelopeState::Release:
            mEnvelopeLevel -= mReleaseIncrement;
            if (mEnvelopeLevel <= 0.0f)
            {
                mEnvelopeLevel = 0.0f;
                mEnvelopeState = EnvelopeState::Idle;
            }
            break;

        case EnvelopeState::Idle:
            break;
    }

    return mEnvelopeLevel;
}
