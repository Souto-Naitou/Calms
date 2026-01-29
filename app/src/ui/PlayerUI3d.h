#pragma once
#include <entity/player/Player.h>
#include <entity/player/PlayerContext.h>
#include <memory>
#include <ui/gauge/RingGauge.h>

class PlayerUI3d
{
public:
    struct Params
    {
        float hp;
        float hpMax;
        float explosionScore;
        float explosionScoreMax;
        float slomoTime;
        float slomoTimeMax;
    };

    PlayerUI3d() = default;
    ~PlayerUI3d() = default;
    void Initialize(DirectX12* pDx12);
    void Finalize();
    void Update(const Params& param);
    void Draw1F();
    void ImGui();

    void SetPosition(const Vector3& pos) { center_ = pos; }

private:
    constexpr static float kGaugeExplosionInnerRadius_  = 4.0f;
    constexpr static float kGaugeHPInnerRadius_         = 3.4f;
    constexpr static float kGaugeSlomoInnerRadius_      = 3.4f;
    constexpr static float kGaugeThickness_             = 0.1f;

    void InitializeGaugeExplosion(DirectX12* pDx12);
    void InitializeGaugeHP(DirectX12* pDx12);
    void InitializeGaugeSlomo(DirectX12* pDx12);

    Vector3                     center_             = {};
    std::unique_ptr<RingGauge>  pGaugeExplosion_    = nullptr;
    std::unique_ptr<RingGauge>  pGaugeHP_           = nullptr;
    std::unique_ptr<RingGauge>  pGaugeSlomo_        = nullptr;
};