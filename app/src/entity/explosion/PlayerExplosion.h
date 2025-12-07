#pragma once
#include <entity/EntityBase.h>
#include <Features/Primitive/RingModel.h>
#include <drawable/Object3d/Object3d.h>
#include <Core/DirectX12/DirectX12.h>
#include <memory>
#include <Features/TimeMeasurer/TimeMeasurerByDt.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Collision/Collider/Collider.h>

class PlayerExplosion : public EntityBase
{
public:
    struct Params
    {
        DirectX12* pDx12 = nullptr;
    };

    PlayerExplosion(const Params& params) : params_(params) {}
    ~PlayerExplosion() override = default;


    void Initialize(const EntityCommonParams& params, bool enableDebugWindow = true) override;


    void Finalize() override;


    void Update() override;


    void Draw1F() override;

private:
    void InitializeRing();
    void InitializeCollider();
    void Expand();
    void UpdateOpacity();
    void UpdateCollider();

    static constexpr uint32_t kDeltaTimeChannelNum = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    static constexpr float kRingThickness           = 0.1f;
    static constexpr float kRingRadiusTarget        = 10.0f;
    static constexpr float kRingLifeTimeSec         = 0.5f;

    Params params_;
    std::unique_ptr<TimeMeasurerByDt>   timeMeasurer_   = nullptr;
    std::unique_ptr<RingModel>          modelRing_      = nullptr;
    std::unique_ptr<Object3d>           objectRing_     = nullptr;
    std::unique_ptr<Collider>           collider_       = nullptr;
    Sphere                              sphere_         = {};


    // Pointers
    CollisionManager* collisionManager_ = nullptr;
};