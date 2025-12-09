#pragma once

class PlayerContext
{
public:
    struct Status
    {
        float explosionScore = 0.0f;
    };

    static constexpr float kMaxExplosionScore = 100.0f;

    void Initialize() {};
    void Finalize() {};

    const Status& Get() const { return status_; }

    void IncreaseExplosionScore(float amount);
    void DecreaseExplosionScore(float amount);
    void ResetExplosionScore() { status_.explosionScore = 0.0f; }

private:
    Status status_;
};