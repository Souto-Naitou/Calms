#include "FollowMovement.h"



FollowMovement::FollowMovement(const Vector3* pTargetPosition)
{
    pTargetPosition_ = pTargetPosition;
}

void FollowMovement::Update(EulerTransform& transform, float deltaTime)
{
    if (pTargetPosition_ == nullptr)
    {
        return;
    }

    Vector3 direction = (*pTargetPosition_ - transform.translate).Normalized();
    /// 期待速度を計算し、現在の速度との差分を力として適用
    Vector3 desiredVelocity = direction * followSpeed_;
    Vector3 steering = desiredVelocity - this->GetVelocity();
    ApplyForce(steering);

    /// 加速度の確定と座標の更新
    PositionUpdate(transform.translate, deltaTime);
}
