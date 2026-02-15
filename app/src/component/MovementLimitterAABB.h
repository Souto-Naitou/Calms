#pragma once
#include <Vector3.h>
#include <Math/Transform.h>
#include <Features/Primitive/AABB.h>

class MovementLimitterAABB
{
public:
    void SetBounds(const AABB* pBounds) { pBounds_ = pBounds; }
    void Update(EulerTransform& transform);
private:
    const AABB* pBounds_ = nullptr;
};