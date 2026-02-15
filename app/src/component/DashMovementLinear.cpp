#include "DashMovementLinear.h"



void DashMovementLinear::StartDash(const Vector3& direction, float distance, float durationSec)
{
    elapsedTime_ = 0.0f;
    isDashing_ = true;
    direction_ = direction;
    distance_ = distance;
    durationSec_ = durationSec;
}

void DashMovementLinear::Update(EulerTransform& transform, float deltaTime)
{
    if (!isDashing_) return;

    /// 1フレームあたりの移動量を計算して適用
    Vector3 deltaPosition = direction_ * (distance_ / durationSec_) * deltaTime;
    transform.translate += deltaPosition;

    /// 時間経過を更新
    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= durationSec_)
    {
        isDashing_ = false;
    }
}

bool DashMovementLinear::IsFinished() const
{
    return !isDashing_;
}
