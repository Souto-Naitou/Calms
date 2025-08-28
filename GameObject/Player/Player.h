#pragma once

#include <GameObject/BaseObject.h>
#include <Features/Object3d/Object3d.h>
#include <memory>
#include <Features/Input/Input.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/Particle/Emitter/ParticleEmitter.h>
#include <Features/Audio/AudioManager.h>
#include <Features/Audio/Audio.h>
#include <Features/Model/ModelManager.h>
#include <Features/Model/IModel.h>

class Player : public BaseObject
{
public:
    Player(ModelManager* _pModelManager)
        : pModelManager_(_pModelManager) {};

    void Initialize(bool _enableDebugWindow = true) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void DrawLine() override;


public: /// Getter
    bool IsShot() const { return isShot_; }
    bool IsSlow() const { return isSlow_; }


public: /// Setter
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }
    void SetEnableInput(bool _enable) { enableInput_ = _enable; }


private:
    void DebugWindow();
    void OnCollisionTrigger(const Collider* _other);
    void OnCollision(const Collider* _other);

    void ObjectsInitialize();
    void ColliderInitialize();

    void UpdateInputCommands();

    std::unique_ptr<IModel>         pModelSelfBody_ = nullptr;
    std::unique_ptr<Object3d>       object_ = {};
    std::unique_ptr<TimeMeasurer>   timerShot_ = {};
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
    std::unique_ptr<ParticleEmitter> shotEmitter_ = nullptr;
    std::shared_ptr<IModel> pModelSpark_ = nullptr;

    /// オーディオ
    Audio* audioShot_ = nullptr;


private: /// 他クラスの所有物
    Input* input_ = nullptr;
    CollisionManager* collisionManager_ = nullptr;
    DeltaTimeManager* deltaTimeManager_ = nullptr;
    AudioManager* audioManager_ = nullptr;
    ModelManager* pModelManager_ = nullptr;
};