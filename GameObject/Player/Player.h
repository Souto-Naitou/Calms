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


public: /// Setter
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }


private:
    std::unique_ptr<Object3d> object_ = {};
    std::unique_ptr<Timer> timerShot_ = {};
    float movePower_ = 0.0f;

    /// 射撃
    bool isShot_ = false;
    double shotInterval_ = 0.1;

    /// コライダー
    std::unique_ptr<Collider> collider_ = nullptr;
    OBB obb_ = {};
    bool isDrawCollisionArea_ = false;


private:
    void UpdateInputCommands();
    void DebugWindow();
    void ModifyGameEye(GameEye* _eye) override;


private: /// 他クラスの所有物
    Input* input_ = nullptr;
    CollisionManager* collisionManager_ = nullptr;
};