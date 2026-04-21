#pragma once
#include <entity/EntityBase.h>
#include <Features/Primitive/RingModel.h>
#include <drawable/Object3d/Object3d.h>
#include <Core/DirectX12/DirectX12.h>
#include <memory>
#include <Features/TimeMeasurer/DeltaTimeStopWatch.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Collision/Collider/Collider.h>

class PlayerExplosion : public EntityBase
{
public:
    struct Params
    {
        DirectX12*  pDx12       = nullptr;
    };

    PlayerExplosion(const Params& params) : params_(params) {}
    ~PlayerExplosion() override = default;

    void Initialize(bool enableDebugWindow = true) override;
    void Finalize() override;
    void Update() override;
    void Draw1F() override;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

private:
    void InitializeRing();
    void InitializeCollider();
    void Expand();
    void UpdateOpacity();
    void UpdateCollider();

    static constexpr uint32_t   kDeltaTimeChannelNum    = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    static constexpr float      kRingThickness          = 3.0f;
    static constexpr float      kRingRadiusTarget       = 10.0f;
    static constexpr float      kRingLifeTimeSec        = 0.5f;

    Params params_;
    EulerTransform                      transform_      = {};
    std::unique_ptr<DeltaTimeStopWatch>   pTimeMeasurer_  = nullptr;
    std::unique_ptr<RingModel>          pModelRing_     = nullptr;
    std::unique_ptr<Object3d>           pObjectRing_    = nullptr;
    std::unique_ptr<Collider>           pCollider_      = nullptr;
    std::unique_ptr<EntityStats>        pStats_         = nullptr;
    Sphere                              sphere_         = {};


    // Pointers
    CollisionManager* collisionManager_ = nullptr;
};