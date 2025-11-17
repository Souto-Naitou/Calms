#pragma once

class PlayerMovement
{
public:
    struct Data
    {
        float speed = 0.0f;
        bool isMoving = false;
    };
    PlayerMovement() = default;
    ~PlayerMovement() = default;
    void Initialize();
    void Update(const Vector3& direction, bool isSlow);
    const Data& GetData() const { return data_; }
};