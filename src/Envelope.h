#pragma once

enum class EnvelopeState { Idle, Attack, Hold, Release };

class Envelope {
public:
    Envelope() = default;
    ~Envelope() = default;

    void updateEnvelopeIncrements(const float sampleRate);
    float updateEnvelope();

    void setState(const EnvelopeState state) { mEnvelopeState = state; }

    void setAttackTime(const float attack);
    void setReleaseTime(const float release);

    float getAttackTime() { return mAttackTimeMs; }
    float getReleaseTime() { return mReleaseTimeMs; }
    EnvelopeState getState() { return mEnvelopeState; }

private:
    inline void updateAttackIncrement();
    inline void updateReleaseIncrement();

    float mAttackIncrement = 0.f;
    float mReleaseIncrement = 0.f;
    float mEnvelopeLevel = 0.f;
    float mSampleRate = 44100.f;
    float mAttackTimeMs = 2.0f;
    float mReleaseTimeMs = 50.0f;
    EnvelopeState mEnvelopeState = EnvelopeState::Idle;
};
