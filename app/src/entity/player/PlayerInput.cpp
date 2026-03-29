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
    
    if (pInput_->IsPadConnected() && pInput_->IsPadUpdated())
    {
        isGamepadMode_ = true;
    }
    
    if (pInput_->IsAnyKeyChanged())
    {
        isGamepadMode_ = false;
    }

    /// [ プレイヤーの入力用に変換 ]
    if (isGamepadMode_)
    {
        auto& iAnalog = pInput_->GetGamepadAnalogInput();
        data_.move = Vector3(iAnalog.thumbL.x, 0.0f, iAnalog.thumbL.y);
        data_.isShotPressed = iAnalog.thumbR.Length() > kShootThreshold_;
        bool isTriggerRightPressed = iAnalog.triggerR > 0.01f;
        bool isTriggerRightPressedPrev = isTriggerRightPressed_;
        data_.isSlowTriggered = isTriggerRightPressed && !isTriggerRightPressedPrev;
        data_.isSlowPressed = isTriggerRightPressed;
        data_.isSlowReleased = isTriggerRightPressedPrev && !isTriggerRightPressed;
        isTriggerRightPressed_ = isTriggerRightPressed;
        data_.isExplosionTriggered = iAnalog.triggerL > 0.01f;
    }
    else
    {
        // 移動
        auto dirX = pInput_->PushKey(DIK_D) - pInput_->PushKey(DIK_A);
        auto dirZ = pInput_->PushKey(DIK_W) - pInput_->PushKey(DIK_S);
        data_.move = Vector3(static_cast<float>(dirX), 0.0f, static_cast<float>(dirZ));
        data_.move = data_.move.Normalized();
        // 射撃
        data_.isShotPressed = pInput_->PushMouse(Input::MouseNum::Left);
        // スロー(トリガー)
        data_.isSlowTriggered = pInput_->TriggerKey(DIK_SPACE);
        // スロー(プレス)
        data_.isSlowPressed = pInput_->PushKey(DIK_SPACE);
        // スロー(リリース)
        data_.isSlowReleased = preData.isSlowPressed && !data_.isSlowPressed;
        // 爆発トリガー
        data_.isExplosionTriggered |= pInput_->TriggerKey(DIK_LSHIFT);
        data_.isExplosionTriggered |= pInput_->TriggerMouse(Input::MouseNum::Right);
    }
}
