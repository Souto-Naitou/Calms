#pragma once

#include <GameObject/BaseObject.h>
#include <Features/Object3d/Object3d.h>
#include <Vector3.h>
#include <memory>
#include <Features/GameEye/GameEye.h>
#include <Timer/Timer.h>

class PlayerBullet : public BaseObject
{
public:
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;


public: /// Setter
    void SetMoveVelocity(const Vector3& _velocity) { moveVelocity_ = _velocity; }

private:
    std::unique_ptr<Object3d> object_ = {};
    std::unique_ptr<Timer> timer_ = {};
    float lifeTimeLimit_ = 3.0f;
    Vector3 moveVelocity_ = {};

private:
    void ModifyGameEye(GameEye* _eye) override;
};