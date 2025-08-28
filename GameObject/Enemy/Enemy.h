#pragma once

#include <GameObject/BaseObject.h>
#include <Features/Object3d/Object3d.h>
#include <Vector3.h>
#include <memory>
#include <Features/GameEye/GameEye.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Primitive/OBB.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/Particle/Emitter/ParticleEmitter.h>
#include <Features/Audio/AudioManager.h>

class Enemy : public BaseObject
{
public:
    struct Desc
    {
        IModel* pModelSelfBody = nullptr;               // 本体モデル
        IModel* pModelParticleHit = nullptr;            // ヒットパーティクルモデル
        IModel* pModelParticleDeath = nullptr;          // デスパーティクルモデル
    };

public:
    Enemy(const Desc& _desc);


    void Initialize(bool _enableDebugWindow = true) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void DrawLine() override;


public: /// Setter
    void SetLocationProvider(BaseObject* _gameObject) { locationProvider_ = _gameObject; }
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }


private:
    void InitializeObjects();
    void InitializeCollider();
    void InitializeParticleEmitters();

    void UpdateTransform();
    void UpdateLights();
    void UpdateCollider();
    void UpdateObjects();

    void OnCollision(const Collider* _other);
    void OnCollisionTrigger(const Collider* _other);
    void DebugWindow() override;

    std::unique_ptr<Object3d>           objectSelfBody_     = {};
    std::unique_ptr<TimeMeasurer>       timeMeasurer_       = {};

    float           lifeTimeLimit_              = 3.0f;
    Vector3         accelerationRefl_           = {};
    Vector3         moveVelocity_               = {};
    Vector2         distanceToTarget            = {};
    BaseObject*     locationProvider_           = nullptr;
    Vector2         positionTarget_             = {};
    Vector2         velocity_move               = {};
    float           moveSpeed_                  = 0.0f;
    float           reflectionPower_            = 15.0f;
    float           bulletReflectionPower_      = 40.0f;

    // Collision
    std::unique_ptr<Collider>   collider_            = nullptr;
    OBB                         obb_                 = {};
    bool                        isDrawCollisionArea_ = false;

    Audio* audioHit_    = nullptr;
    Audio* audioDeath_  = nullptr;

    /// パーティクル
    std::unique_ptr<ParticleEmitter>    pParticleHit_               = nullptr;
    std::unique_ptr<ParticleEmitter>    pParticleDeath_             = nullptr;
    std::unique_ptr<IModel>             pModelSelfBody_             = nullptr;
    IModel*                             pModelParticleHit_          = nullptr;
    IModel*                             pModelParticleDeath_        = nullptr;


private: /// 他クラスの所有物
    CollisionManager*   collisionManager_   = nullptr;
    DeltaTimeManager*   deltaTimeManager_   = nullptr;
    AudioManager*       audioManager_       = nullptr;
};