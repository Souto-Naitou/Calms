#pragma once
#include <Math/Transform.h>

class IMovement
{
public:
    virtual ~IMovement() = default;
    virtual void Update(EulerTransform& transform, float deltaTime) = 0;
};