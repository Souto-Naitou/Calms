#include "PlayerBulletGenerator.h"
#include <numbers>
#include <Features/RandomGenerator/RandomGenerator.h>



std::vector<std::unique_ptr<PlayerBullet>> 
PlayerBulletGenerator::Generate(const Vector3& standard, const Vector3& direction)
{
    if (!config_.pParticle) assert(false && "パーティクルが設定されていません。");

    std::vector<std::unique_ptr<PlayerBullet>> bullets;

    const int32_t   kNumShots       = config_.numShot;
    const float     kSpreadAngleDeg = config_.spreadAngleDeg;
    // ゼロ除算防止 (0度のときは散らさない)
    const float     kSpreadRad      = kSpreadAngleDeg == 0.0f ? 0.0f : std::numbers::pi_v<float> / (360.0f / kSpreadAngleDeg);
    const float     kBulletSpeed    = config_.bulletSpeed;
    const float     kSwingSize      = config_.swingSize;
    RandomGenerator* randomGenerator_ = RandomGenerator::GetInstance();

    //Vector3 direction = screenToWorld_->GetWorldPoint() - pPlayer_->GetTranslation();
    for (int32_t i = 0; i < kNumShots; ++i)
    {
        // -15°〜15°の範囲で散らす
        int32_t index = i - (kNumShots / 2);
        float angle = kSpreadRad * static_cast<float>(index);

        Vector3 newDirection = {};
        newDirection.x = direction.x * std::cosf(angle) - direction.z * std::sinf(angle);
        newDirection.y = 0.0f;
        newDirection.z = direction.x * std::sinf(angle) + direction.z * std::cosf(angle);
        newDirection = newDirection.Normalized();
        newDirection.x += randomGenerator_->Generate(-kSwingSize, kSwingSize);
        newDirection.z += randomGenerator_->Generate(-kSwingSize, kSwingSize);
        newDirection = newDirection.Normalized();

        auto& pParticle = config_.pParticle;
        pParticle->emplace_back({});

        auto bullet = std::make_unique<PlayerBullet>(
            PlayerBullet::Params{ &pParticle->GetParticleData().back() }
        );
        bullet->Initialize({}, false);
        bullet->SetTranslation(standard);
        bullet->SetMoveVelocity(newDirection * kBulletSpeed);
        bullet->SetIsDrawCollisionArea(false);
        bullets.push_back(std::move(bullet));
    }
    return bullets;
}
