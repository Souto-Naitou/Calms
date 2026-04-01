#pragma once

/// <summary>
/// プレイヤーの基本情報や現在の状態を管理するコンテキストクラス
/// </summary>
class PlayerContext
{
public:
    struct Status
    {
        float explosionScore = 0.0f;
    };

    static constexpr float kMaxExplosionScore = 160.0f;

    void Initialize() {};
    void Finalize() {};

    const Status& Get() const { return status_; }

    void IncreaseExplosionScore(float amount);
    void DecreaseExplosionScore(float amount);
    void ResetExplosionScore() { status_.explosionScore = 0.0f; }

private:
    Status status_;
};