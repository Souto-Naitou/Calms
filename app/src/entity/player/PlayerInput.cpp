#include "PlayerInput.h"



void PlayerInput::Initialize()
{
    pInput_ = Input::GetInstance();
}

void PlayerInput::Update()
{
    // 前回の入力データ
    PlayerInput::Data preData = data_;
    data_ = {};

    /// [ プレイヤーの入力用に変換 ]
    // 移動
    auto dirX = pInput_->PushKey(DIK_D) - pInput_->PushKey(DIK_A);
    auto dirZ = pInput_->PushKey(DIK_W) - pInput_->PushKey(DIK_S);
    data_.move = Vector3(static_cast<float>(dirX), 0.0f, static_cast<float>(dirZ));
    data_.move = data_.move.Normalized();
    // 射撃
    data_.isShotPressed = pInput_->PushMouse(Input::MouseNum::Left);
    // スロー(トリガー)
    data_.isSlowTriggered = pInput_->TriggerKey(DIK_LSHIFT);
    // スロー(プレス)
    data_.isSlowPressed = pInput_->PushKey(DIK_LSHIFT);
    // スロー(リリース)
    data_.isSlowReleased = preData.isSlowPressed && !data_.isSlowPressed;
    // 爆発トリガー
    data_.isExplosionTriggered |= pInput_->TriggerKey(DIK_SPACE);
    data_.isExplosionTriggered |= pInput_->TriggerMouse(Input::MouseNum::Right);
}
