#include "InputGuide.h"

void InputGuide::Initialize()
{
    leftclick_ = new Sprite();
    leftclick_->SetName("leftclick");
    leftclick_->Initialize("leftclick.png");
    leftclick_->SetAnchorPoint({ 0.0f, 0.5f });
    leftclick_->SetPosition({ 42.0f, 835.0f });

    shift_ = new Sprite();
    shift_->SetName("shift");
    shift_->Initialize("shift.png");
    shift_->SetAnchorPoint({ 0.0f, 0.5f });
    shift_->SetPosition({ 42.0f, 764.0f });
}

void InputGuide::Update()
{
    leftclick_->Update();
    shift_->Update();
}

void InputGuide::Draw()
{
    leftclick_->Draw();
    shift_->Draw();
}

void InputGuide::Finalize()
{
    leftclick_->Finalize();
    delete leftclick_;

    shift_->Finalize();
    delete shift_;
}
