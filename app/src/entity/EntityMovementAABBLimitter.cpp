#include "EntityMovementAABBLimitter.h"
#include <algorithm>



void EntityMovementAABBLimitter::Update(EulerTransform& transform)
{
    if (pBounds_ == nullptr) return;

    Vector3& translate = transform.translate;
    const Vector3 kScaleHalf = transform.scale * 0.5f;

    const Vector3& minPos = pBounds_->GetMin();
    const Vector3& maxPos = pBounds_->GetMax();

    translate.x = std::clamp(translate.x, minPos.x + kScaleHalf.x, maxPos.x - kScaleHalf.x);
    translate.y = std::clamp(translate.y, minPos.y + kScaleHalf.y, maxPos.y - kScaleHalf.y);
    translate.z = std::clamp(translate.z, minPos.z + kScaleHalf.z, maxPos.z - kScaleHalf.z);
}
