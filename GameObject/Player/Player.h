#pragma once

#include <GameObject/BaseObject.h>
#include <Features/Object3d/Object3d.h>
#include <Features/gameEye/GameEye.h>
#include <memory>
#include <Features/Input/Input.h>
#include <Timer/Timer.h>
#include <Collision/Manager/CollisionManager.h>
#include <Collision/Collider/Collider.h>
#include <Collision/Shape.h>
#include <DeltaTimeManager/DeltaTimeManager.h>
#include <Features/Particle/Emitter/ParticleEmitter.h>

class Player : public BaseObject
{
public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();
    void DrawLine();


public: /// Getter
    bool IsShot() const { return isShot_; }
    bool IsSlow() const { return isSlow_; }


public: /// Setter
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }
    void SetEnableInput(bool _enable) { enableInput_ = _enable; }


private:
    std::unique_ptr<Object3d> object_ = {};
    std::unique_ptr<Timer> timerShot_ = {};
    float movePower_ = 0.0f;

    /// 射撃
    bool isShot_ = false;
    double shotInterval_ = 0.05;

    /// スロー
    bool isSlow_ = false;

    /// コライダー
    std::unique_ptr<Collider> collider_ = nullptr;
    OBB obb_ = {};
    bool isDrawCollisionArea_ = false;

    /// 反発用
    Vector3 accelerationRefl_ = {};
    float reflectionPower_ = 70.0f;

    /// マルチプレイ用
    bool enableInput_ = true;

    /// パーティクルエミッター
    std::unique_ptr<ParticleEmitter> shotEmitter = nullptr;

private:
    void UpdateInputCommands();
    void DebugWindow();
    void ModifyGameEye(GameEye* _eye) override;
    void OnCollisionTrigger(const Collider* _collider);
    void OnCollision(const Collider* _collider);


private: /// 他クラスの所有物
    Input* input_ = nullptr;
    CollisionManager* collisionManager_ = nullptr;
    DeltaTimeManager* deltaTimeManager_ = nullptr;
};