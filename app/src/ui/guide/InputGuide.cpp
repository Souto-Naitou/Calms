#include "InputGuide.h"
#include <config/ResourcePath.h>

void InputGuide::Initialize(Canvas* canvas)
{
    canvas_ = canvas;

    leftclick_ = std::make_unique<Sprite>();
    leftclick_->Initialize(Path::Image::kGuideShoot);
    leftclick_->SetName("leftclick");
    leftclick_->SetAnchorPoint({ 0.0f, 0.5f });
    leftclick_->SetPosition({ 42.0f, 835.0f });
    canvas->RegisterDrawable(leftclick_.get());

    shift_ = std::make_unique<Sprite>();
    shift_->Initialize(Path::Image::kGuideSlow);
    shift_->SetName("shift");
    shift_->SetAnchorPoint({ 0.0f, 0.5f });
    shift_->SetPosition({ 42.0f, 764.0f });
    canvas->RegisterDrawable(shift_.get());
}

void InputGuide::Update()
{
    leftclick_->Update();
    shift_->Update();
}

void InputGuide::Draw()
{
    leftclick_->Draw1F();
    shift_->Draw1F();
}

void InputGuide::Finalize()
{
    leftclick_->Finalize();
    shift_->Finalize();
    canvas_->UnregisterDrawable(leftclick_.get());
    canvas_->UnregisterDrawable(shift_.get());
}
