#pragma once

enum class EnvelopeState { Idle, Attack, Hold, Release };

class Envelope {
public:
    Envelope() = default;
    ~Envelope() = default;

    void updateEnvelopeIncrements(const float sampleRate);
    float updateEnvelope();

    void setState(const EnvelopeState state) { mEnvelopeState = state; }

    void setAttack(const float attack)
    { 
        mAttackTimeMs = attack;
        updateEnvelopeIncrements(mSampleRate);
    }

    void setRelease(const float release) 
    { 
        mReleaseTimeMs = release;
        updateEnvelopeIncrements(mSampleRate);
    }

    float getAttack() { return mAttackTimeMs; }
    float getRelease() { return mReleaseTimeMs; }
    EnvelopeState getState() { return mEnvelopeState; }

private:
    float mAttackIncrement = 0.f;
    float mReleaseIncrement = 0.f;
    float mEnvelopeLevel = 0.f;
    float mSampleRate = 44100.f;
    float mAttackTimeMs = 2.0f;
    float mReleaseTimeMs = 50.0f;
    EnvelopeState mEnvelopeState = EnvelopeState::Idle;
};
