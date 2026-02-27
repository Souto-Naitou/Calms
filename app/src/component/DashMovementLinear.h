#pragma once
#include <component/IMovement.h>
#include <memory>

/// <summary>
/// ダッシュ移動(直線移動)
/// </summary>
class DashMovementLinear : public IMovement
{
public:
    void StartDash(const Vector3& direction, float distance, float durationSec);
    void Update(EulerTransform& transform, float deltaTime) override;
    bool IsFinished() const;
    float GetElapsedTime() const { return elapsedTime_; }

private:
    Vector3 direction_      = { 1.0f, 0.0f, 0.0f };
    float   distance_       = 0.0f;
    float   durationSec_    = 0.0f;
    float   elapsedTime_    = 0.0f;
    bool    isDashing_      = false;
};