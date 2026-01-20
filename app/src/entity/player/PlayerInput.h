#pragma once
#include <Features/Input/Input.h>
#include <Vector3.h>

/// <summary>
/// プレイヤー入力管理クラス
/// </summary>
class PlayerInput
{
public:
    struct Data
    {
        bool    isSlowPressed           = false;
        bool    isShotPressed           = false;
        bool    isSlowTriggered         = false;
        bool    isSlowReleased          = false;
        bool    isExplosionTriggered    = false;
        // 移動方向ベクトル (PlayerInput::Update()で正規化済み)
        Vector3 move = {};
    };

    PlayerInput() = default;
    ~PlayerInput() = default;

    void Initialize();
    void Update();

    const Data& GetData() const { return data_; }

private:
    Input* pInput_ = nullptr;
    Data data_ = {};
};