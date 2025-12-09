#pragma once
#include <entity/player/Player.h>
#include <entity/player/PlayerContext.h>
#include <memory>
#include <ui/gauge/RingGauge.h>

class PlayerUI3d
{
public:
    PlayerUI3d() = default;
    ~PlayerUI3d() = default;
    void Initialize(Player* player, DirectX12* pDx12);
    void Finalize();
    void Update();
    void Draw1F();
    void ImGui();

    void SetPosition(const Vector3& pos) { position_ = pos; }

private:
    void InitializeGaugeExplosion(DirectX12* pDx12);
    void InitializeGaugeHP(DirectX12* pDx12);

    Vector3                     position_           = {};
    const PlayerContext*        pContext_           = nullptr;
    const EntityStats*          pStats_             = nullptr;
    std::unique_ptr<RingGauge>  gaugeExplosion_     = nullptr;
    std::unique_ptr<RingGauge>  gaugeHP_            = nullptr;
};