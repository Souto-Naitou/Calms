#pragma once

#include <Vector3.h>
#include <string>
#include <Features/GameEye/GameEye.h>
#include <DIContainer/DIContainer.h>
#include <Features/Lighting/PointLight/PointLight.h>

class BaseObject
{
public:
    BaseObject() = default;
    virtual ~BaseObject() = default;
    virtual void Initialize(bool _enableDebugWindow = true);
    virtual void Finalize();
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void DrawLine() = 0;


public: /// Getter
    const std::string& GetName() const { return name_; }
    const Vector3& GetRotation() const { return rotation_; }
    const Vector3& GetScale() const { return scale_; }
    const Vector3& GetTranslation() const { return translation_; }
    const Vector3& GetVelocity() const { return velocity_; }
    const Vector3& GetAcceleration() const { return acceleration_; }
    float GetFriction() const { return friction_; }
    const bool IsAlive() const { return isAlive_; }
    const float GetAttackPower() const { return attackPower_; }


public: /// Setter
    void SetName(const std::string& _name) { name_ = _name; }
    void SetRotation(const Vector3& _rotation) { rotation_ = _rotation; }
    void SetScale(const Vector3& _scale) { scale_ = _scale; }
    void SetTranslation(const Vector3& _translation) { translation_ = _translation; }
    void SetVelocity(const Vector3& _velocity) { velocity_ = _velocity; }
    void SetAcceleration(const Vector3& _acceleration) { acceleration_ = _acceleration; }
    void SetFriction(float _friction) { friction_ = _friction; }
    void SetDIContainer(DIContainer* _diContainer) { diContainer_ = _diContainer; }


protected:
    std::string name_ = "unnamed";

    bool    isEnableDebugWindow_    = true;
    bool    isAlive_                = true;

    float   hp_                     = 0.0f;

    Vector3 rotation_               = {};
    Vector3 scale_                  = {};
    Vector3 translation_            = {};
    Vector3 velocity_               = {};
    Vector3 acceleration_           = {};

    float   friction_               = 1.0f;
    float   attackPower_            = 0.0f;


protected:
    void UpdateTransform(float _dt);
    virtual void DebugWindow();


protected: /// 他クラスの所有物
    GameEye** ppGameEye_ = nullptr;
    DIContainer* diContainer_ = nullptr;
    DirectionalLight* directionalLight_ = nullptr;
    PointLight* pointLight_ = nullptr;
};