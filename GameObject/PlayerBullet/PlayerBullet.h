#pragma once

#include <GameObject/BaseObject.h>
#include <Features/Object3d/Object3d.h>
#include <Vector3.h>
#include <memory>
#include <Features/GameEye/GameEye.h>
#include <Timer/Timer.h>
#include <Collision/Collider/Collider.h>
#include <Collision/Manager/CollisionManager.h>
#include <Features/Primitive/OBB.h>
#include <DeltaTimeManager/DeltaTimeManager.h>

class PlayerBullet : public BaseObject
{
public:
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void DrawLine() override;


public: /// Setter
    void SetMoveVelocity(const Vector3& _velocity) { moveVelocity_ = _velocity; }
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }


private:
    std::unique_ptr<Object3d> object_ = nullptr;
    std::unique_ptr<Timer> timer_ = nullptr;
    float lifeTimeLimit_ = 8.0f;
    Vector3 moveVelocity_ = {};

    /// コライダー
    std::unique_ptr<Collider> collider_ = nullptr;
    OBB obb_ = {};
    bool isDrawCollisionArea_ = false;

private:
    void ModifyGameEye(GameEye* _eye) override;
    void OnCollisionTrigger(const Collider* _other);
    void DebugWindow() override;

private: /// 他クラスの所有物
    CollisionManager* collisionManager_ = nullptr;
    DeltaTimeManager* deltaTimeManager_ = nullptr;
};