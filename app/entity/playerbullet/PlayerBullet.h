#pragma once

#include <Entity/EntityBase.h>
#include <Features/Object3d/Object3d.h>
#include <Vector3.h>
#include <memory>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Primitive/OBB.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>


class PlayerBullet : public EntityBase
{
public:
    PlayerBullet(IModel* _pModelSelfBody)
        : pModelSelfBody_(_pModelSelfBody) {};

    void Initialize(bool _enableDebugWindow = true) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void DrawLine() override;
    void ImGui() override;


public: /// Setter
    void SetMoveVelocity(const Vector3& _velocity) { moveVelocity_ = _velocity; }
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }


private:
    /// コールバック関数
    void OnCollisionTrigger(const Collider* _other);

    /// 内部関数
    void ObjectsInitialize();
    void ObjectsUpdate();
    void CollidersInitialize();

    std::unique_ptr<TimeMeasurer> timer_ = nullptr;
    
    IModel* pModelSelfBody_ = nullptr;
    std::unique_ptr<Object3d> pBody_ = nullptr;
    
    /// パラメータ
    float lifeTimeLimit_ = 8.0f;
    Vector3 moveVelocity_ = {};

    /// コライダー
    std::unique_ptr<Collider> collider_ = nullptr;
    OBB obb_ = {};

    /// フラグ
    bool isDrawCollisionArea_ = false;


private: /// 他クラスの所有物
    CollisionManager* collisionManager_ = nullptr;
    DeltaTimeManager* deltaTimeManager_ = nullptr;
};