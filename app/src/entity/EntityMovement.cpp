#include "EntityMovement.h"

void EntityMovement::Update(EulerTransform& transform, float deltaTime)
{
    this->PositionUpdate(transform.translate, deltaTime);
}

void EntityMovement::AddImpulse(const Vector3& impulse)
{
    velocity_ += impulse;
}

void EntityMovement::StopMovement()
{
    velocity_ = Vector3();
    acceleration_ = Vector3();
}

void EntityMovement::PositionUpdate(Vector3& position, float deltaTime)
{
    velocity_ += acceleration_ * deltaTime;
    position += velocity_ * deltaTime;
    acceleration_ = Vector3();
}
