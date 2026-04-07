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
    
    /// [ ゲームパッドモードの切り替え ]


    /// [ プレイヤーの入力用に変換 ]
    if (pInput_->IsPadMode())
    {
        // アナログスティックの入力が小さい場合は0とみなすための閾値
        constexpr float minAnalogLength = 0.01f;

        // 移動
        auto& iAnalog = pInput_->GetGamepadAnalogInput();
        data_.move = Vector3(iAnalog.thumbL.x, 0.0f, iAnalog.thumbL.y);

        // 射撃
        const float shootInputMagnitude = iAnalog.thumbR.LengthWithoutRoot();
        const float shootThresholdSquared = kShootThreshold_ * kShootThreshold_;
        data_.isShotPressed = shootInputMagnitude > shootThresholdSquared;

        // スロー(トリガー/プレス/リリース)と爆発トリガー
        const bool currTriggerR = iAnalog.triggerR > minAnalogLength;
        const bool prevTriggerR = isTriggerRightPressed_;
        const bool currTriggerL = iAnalog.triggerL > minAnalogLength;
        const bool prevTriggerL = isTriggerLeftPressed_;
        data_.isSlowTriggered = currTriggerR && !prevTriggerR;
        data_.isSlowPressed = currTriggerR;
        data_.isSlowReleased = prevTriggerR && !currTriggerR;
        data_.isExplosionTriggered= currTriggerL && !prevTriggerL;

        // 現在のトリガーの状態を保存 (次フレームで比較するため)
        isTriggerRightPressed_ = currTriggerR;
        isTriggerLeftPressed_ = currTriggerL;
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
