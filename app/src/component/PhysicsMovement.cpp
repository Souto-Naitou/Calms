#include "PhysicsMovement.h"

void PhysicsMovement::Update(EulerTransform& transform, float deltaTime)
{
    this->PositionUpdate(transform.translate, deltaTime);
}

void PhysicsMovement::AddImpulse(const Vector3& impulse)
{
    velocity_ += impulse;
}

void PhysicsMovement::StopMovement()
{
    velocity_ = Vector3();
    acceleration_ = Vector3();
}

void PhysicsMovement::PositionUpdate(Vector3& position, float deltaTime)
{
    velocity_ += acceleration_ * deltaTime;
    position += velocity_ * deltaTime;
    acceleration_ = Vector3();
}
