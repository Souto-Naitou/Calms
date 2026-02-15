#include "EntityFocusOrientation.h"
#include <cmath>
#include <numbers>


void EntityFocusOrientation::Update(EulerTransform& transform, float deltaTime)
{
    constexpr float kPi = std::numbers::pi_v<float>;

    if (pTargetPosition_ == nullptr) return;

    Vector3 direction = (*pTargetPosition_ - transform.translate).Normalized();
    Vector3 targetRotation = {};
    targetRotation.x = std::atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    targetRotation.y = std::atan2f(direction.x, direction.z);
    // 回転の補間
    Vector3 rotationDiff = targetRotation - transform.rotate;
    // 角度の差を-πからπの範囲に収める
    rotationDiff.x = std::fmodf(rotationDiff.x + kPi, 2.0f * kPi) - kPi;
    rotationDiff.y = std::fmodf(rotationDiff.y + kPi, 2.0f * kPi) - kPi;
    transform.rotate.x += rotationDiff.x * rotateRatioPerSec_ * deltaTime;
    transform.rotate.y += rotationDiff.y * rotateRatioPerSec_ * deltaTime;
}
