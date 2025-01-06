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

class Enemy : public BaseObject
{
public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();
    void DrawLine();


public: /// Setter
    void SetLocationProvider(BaseObject* _gameObject) { locationProvider_ = _gameObject; }
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }


private:
    std::unique_ptr<Object3d> object_ = {};
    std::unique_ptr<Timer> timer_ = {};
    float lifeTimeLimit_ = 3.0f;
    Vector3 accelerationRefl_ = {};
    Vector3 moveVelocity_ = {};
    Vector2 distanceToTarget = {};
    BaseObject* locationProvider_ = nullptr;
    Vector2 positionTarget_ = {};
    Vector2 velocity_move = {};
    float moveSpeed_ = 0.0f;
    float reflectionPower_ = 15.0f;

    std::unique_ptr<Collider> collider_ = nullptr;
    OBB obb_ = {};
    bool isDrawCollisionArea_ = false;


private:
    void ModifyGameEye(GameEye* _eye) override;
    void OnCollision(const Collider* _other);
    void OnCollisionTrigger(const Collider* _other);
    void DebugWindow() override;


private: /// 他クラスの所有物
    CollisionManager* collisionManager_ = nullptr;
};