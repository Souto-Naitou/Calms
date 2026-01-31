#pragma once
#include <Vector3.h>
#include <Math/Transform.h>

class EntityFocusOrientation
{
public:
    void SetTargetPosition(const Vector3* pPosition) { pTargetPosition_ = pPosition; }
    void SetRotateRatio(float ratioPerSec) { rotateRatioPerSec_ = ratioPerSec; }
    void Update(EulerTransform& transform, float deltaTime);

private:
    float rotateRatioPerSec_ = 1.0f;
    const Vector3* pTargetPosition_ = nullptr;
};