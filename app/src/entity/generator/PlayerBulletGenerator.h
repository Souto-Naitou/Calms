#pragma once
#include <drawable/particle/Particle.h>
#include <entity/playerbullet/PlayerBullet.h>
#include <vector>
#include <memory>

/// <summary>
/// プレイヤー弾生成クラス
/// </summary>
class PlayerBulletGenerator
{
public:
    struct Config
    {
        Particle*   pParticle       = nullptr;
        int32_t     numShot         = 1;
        float       spreadAngleDeg  = 0.0f;
        float       bulletSpeed     = 30.0f;
        float       swingSize       = 0.0f;
    };

    void SetConfig(const Config& config)
    {
        config_ = config;
    }

    std::vector<std::unique_ptr<PlayerBullet>>
    Generate(const Vector3& standard, const Vector3& direction);

private:
    // 発射に必要なパラメータ
    Config config_;
};