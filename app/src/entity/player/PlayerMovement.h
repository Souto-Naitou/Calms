#pragma once
#include <Vector3.h>
#include <Math/Transform.h>
#include "PlayerInput.h"

class PlayerMovement
{
public:
    struct Data
    {
        Vector3 acceleration = {};
        Vector3 velocity = {};

        float movePower = 0.0f;
        float friction = 1.0f;
    };

    PlayerMovement() = default;
    ~PlayerMovement() = default;
    void Initialize(PlayerInput* pInput, EulerTransform* pTransform);
    void Update(float dt);

    const auto& GetData() const { return data_; }

    inline void AddAcceleration(const Vector3& acc) { data_.acceleration += acc; }

private:
    void UpdateByInput(float dt);
    void ApplyPhysics(float dt);

    Data data_ = {};
    PlayerInput* pInput_ = nullptr;
    EulerTransform* pTransform_ = nullptr;
};