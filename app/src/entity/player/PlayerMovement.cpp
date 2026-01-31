#include "PlayerMovement.h"
#include <cassert>
#include <algorithm>


void PlayerMovement::Update(EulerTransform& transform, float deltaTime)
{
    // 入力による加速度の更新
    this->UpdateByInput(deltaTime);

    // 位置の更新 (加速度と速度から算出)
    EntityMovement::PositionUpdate(transform.translate, deltaTime);
}

bool PlayerMovement::IsMove(float speedThreshold /*= 0.0f*/) const
{
    const float speedSq = EntityMovement::GetVelocity().LengthWithoutRoot();
    const float thresholdSq = speedThreshold * speedThreshold;
    return speedSq > thresholdSq;
}

void PlayerMovement::UpdateByInput(float dt)
{
    const auto& inputData = pInput_->GetData();
    ApplyForce(inputData.move * movePower_);
}