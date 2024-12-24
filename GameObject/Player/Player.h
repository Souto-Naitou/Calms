#pragma once

#include <GameObject/BaseObject.h>
#include <Features/Object3d/Object3d.h>
#include <Features/gameEye/GameEye.h>
#include <memory>
#include <Features/Input/Input.h>
#include <Timer/Timer.h>

class Player : public BaseObject
{
public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();


public: /// Getter
    bool IsShot() const { return isShot_; }


public: /// Setter


private:
    std::unique_ptr<Object3d> object_ = {};
    std::unique_ptr<Timer> timerShot_ = {};
    float movePower_ = 0.0f;

    /// 射撃
    bool isShot_ = false;
    double shotInterval_ = 0.1;


private:
    void UpdateInputCommands();
    void DebugWindow();
    void ModifyGameEye(GameEye* _eye) override;


private: /// 他クラスの所有物
    Input* input_ = nullptr;
};