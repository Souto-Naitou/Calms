#include "InputGuide.h"
#include <config/ResourcePath.h>
#include <Core/Win32/WinSystem.h>

void InputGuide::Initialize(Canvas* canvas)
{
    canvas_ = canvas;

    const float margin = WinSystem::clientWidth / 40.0f;
    float cursorY = WinSystem::clientHeight - margin;

    leftclick_ = std::make_unique<Sprite>();
    leftclick_->Initialize(Path::Image::kGuideShoot);
    leftclick_->SetName("leftclick");
    leftclick_->SetAnchorPoint({ 0.0f, 0.5f });
    cursorY -= leftclick_->GetSize().y / 2.0f;
    leftclick_->SetPosition({ margin, cursorY });

    cursorY -= leftclick_->GetSize().y + margin;
    shift_ = std::make_unique<Sprite>();
    shift_->Initialize(Path::Image::kGuideSlow);
    shift_->SetName("shift");
    shift_->SetAnchorPoint({ 0.0f, 0.5f });
    shift_->SetPosition({ margin, cursorY });
}

void InputGuide::Update()
{
    leftclick_->Update();
    shift_->Update();
}

void InputGuide::Draw1F()
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
