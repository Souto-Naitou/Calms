#include "PlayerInput.h"



void PlayerInput::Initialize()
{
    pInput_ = Input::GetInstance();
}

void PlayerInput::Update()
{
    PlayerInput::Data preData = data_;
    data_ = {};

    if (pInput_->PushKey(DIK_W))
    {
        data_.move.z += 1.0f;
    }
    if (pInput_->PushKey(DIK_S))
    {
        data_.move.z -= 1.0f;
    }
    if (pInput_->PushKey(DIK_A))
    {
        data_.move.x -= 1.0f;
    }
    if (pInput_->PushKey(DIK_D))
    {
        data_.move.x += 1.0f;
    }

    if (pInput_->PushMouse(Input::MouseNum::Left))
    {
        data_.isShotPressed = true;
    }

    if (pInput_->TriggerKey(DIK_LSHIFT))
    {
        data_.isSlowTriggered = true;
    }

    if (pInput_->PushKey(DIK_LSHIFT))
    {
        data_.isSlowPressed = true;
    }

    if (preData.isSlowPressed && !data_.isSlowPressed)
    {
        data_.isSlowReleased = true;
    }
}
