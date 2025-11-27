#include "PlayerMovement.h"
#include <cassert>
#include <algorithm>



void PlayerMovement::Initialize(PlayerInput* pInput, EulerTransform* pTransform)
{
    pTransform_ = pTransform;
    pInput_ = pInput;
}

void PlayerMovement::Update(float dt)
{
    if (!isEnabled_) return;

    if (!pTransform_) 
    { 
        assert(false && "pTransform is nullptr"); 
        return; 
    }

    // 入力による加速度の更新
    this->UpdateByInput(dt);

    // 速度や位置の確定
    this->ApplyPhysics(dt);
}

void PlayerMovement::UpdateByInput(float dt)
{
    const auto& inputData = pInput_->GetData();
    data_.acceleration += inputData.move * data_.movePower;
}

void PlayerMovement::ApplyPhysics(float dt)
{
    /// [ 各種パラメータ適用 ]
    // 加速度から速度を更新
    data_.velocity += data_.acceleration * dt;
    // 摩擦を0.0f〜1.0fの範囲に制限
    data_.friction = std::clamp(data_.friction, 0.0f, 1.0f);
    // 速度に摩擦を適用
    data_.velocity *= data_.friction;

    /// [ 出力 ]
    // 速度から位置を更新
    pTransform_->translate += data_.velocity * dt;
    // 加速度をリセット
    data_.acceleration = {};
}
