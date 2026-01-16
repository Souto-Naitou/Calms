#pragma once
#include <Vector3.h>
#include <Math/Transform.h>
#include "PlayerInput.h"

/// <summary>
/// プレイヤーの移動処理 実装クラス
/// </summary>
class PlayerMovement
{
public:
    struct Data
    {
        Vector3 acceleration    = {};
        Vector3 velocity        = {};
        float movePower         = 25.0f;
        float friction          = 0.95f;
    };

    PlayerMovement() = default;
    ~PlayerMovement() = default;
    void Initialize(PlayerInput* pInput, EulerTransform* pTransform);
    void Update(float dt);

    const auto& GetData() const { return data_; }
    bool IsMove(float threshold = 0.0f) const;

    inline void AddAcceleration(const Vector3& acc) { data_.acceleration += acc; }
    inline void SetEnable(bool flag) { isEnabled_ = flag; }

private:
    void UpdateByInput(float dt);
    void ApplyPhysics(float dt);

    bool            isEnabled_  = true;
    Data            data_       = {};
    PlayerInput*    pInput_     = nullptr;
    EulerTransform* pTransform_ = nullptr;
};