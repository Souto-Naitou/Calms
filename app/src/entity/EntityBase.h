#pragma once

#include <Vector3.h>
#include <Features/GameEye/GameEye.h>
#include <DIContainer/DIContainer.h>
#include <Features/Lighting/PointLight/PointLight.h>
#include <Entity/Status/EntityStats.h>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <Interfaces/IEntityStats.h>
#include <memory>

class EntityBase
{
public:
    EntityBase() = default;
    virtual ~EntityBase() = default;
    virtual void Initialize(bool enableDebugWindow = true);
    virtual void Finalize() {};
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void DrawLine() = 0;
    virtual void ImGui();


public: /// Getter
    const Vector3& GetRotation() const { return rotation_; }
    const Vector3& GetScale() const { return scale_; }
    const Vector3& GetTranslation() const { return translation_; }
    const Vector3& GetVelocity() const { return velocity_; }
    const Vector3& GetAcceleration() const { return acceleration_; }
    float GetFriction() const { return friction_; }
    const bool IsAlive() const { return isAlive_; }
    const float GetAttackPower() const { return attackPower_; }
    const IEntityStats* GetStats() const { return &stats_; }


public: /// Setter
    void SetRotation(const Vector3& rotation) { rotation_ = rotation; }
    void SetScale(const Vector3& scale) { scale_ = scale; }
    void SetTranslation(const Vector3& translation) { translation_ = translation; }
    void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
    void SetAcceleration(const Vector3& acceleration) { acceleration_ = acceleration; }
    void SetFriction(float friction) { friction_ = friction; }
    void SetDIContainer(DIContainer* diContainer) { diContainer_ = diContainer; }


protected:
    std::unique_ptr<DebugEntry<EntityBase>> pDebugEntry_ = {};

    bool    isEnableDebugWindow_    = true;
    bool    isAlive_                = true;

    EntityStats  stats_             = {};

    Vector3 rotation_               = {};
    Vector3 scale_                  = {};
    Vector3 translation_            = {};
    Vector3 velocity_               = {};
    Vector3 acceleration_           = {};

    float   friction_               = 1.0f;
    float   attackPower_            = 0.0f;


protected:
    void UpdatePhysics(float dt);


protected: /// 他クラスの所有物
    GameEye** ppGameEye_ = nullptr;
    DIContainer* diContainer_ = nullptr;
    DirectionalLight* directionalLight_ = nullptr;
    PointLight* pointLight_ = nullptr;
};