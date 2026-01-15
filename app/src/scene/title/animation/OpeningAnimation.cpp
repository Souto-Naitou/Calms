#include "OpeningAnimation.h"
#include <Core/Window/Window.h>
#include <Vector2.h>


OpeningAnimation::OpeningAnimation()
{
    pDebugEntry_ = std::make_unique<DebugEntry<OpeningAnimation>>("AnimationScene", "OpeningAnimation", this, false);
}

OpeningAnimation::~OpeningAnimation()
{
}

void OpeningAnimation::Initialize()
{
    const Vector2 clientSize = 
    { 
        static_cast<float>(Window::clientWidth), 
        static_cast<float>(Window::clientHeight) 
    };

    // Spriteの初期化
    spriteBackground_ = std::make_unique<Sprite>();
    spriteBackground_->Initialize("white1x1.png");
    spriteBackground_->SetSize(clientSize);
    spriteBackground_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

    // Tweenの生成
    AnimationTween<float> tweenOpacity(1.0f, 3.0f, 1.0f, 0.0f);

    // AnimationTimelineの初期化
    timelineOpacity_ = std::make_unique<AnimationTimeline<float>>();
    timelineOpacity_->AddTween(tweenOpacity);
}

void OpeningAnimation::Update()
{
    // AnimationTimelineの更新
    const float opacity = timelineOpacity_->Update();

    // Spriteの更新
    const Vector3 color = spriteBackground_->GetColor().xyz();
    spriteBackground_->SetColor(Vector4(color, opacity));
    spriteBackground_->Update();
}

void OpeningAnimation::Draw1F()
{
    // Spriteの描画
    spriteBackground_->Draw1F();
}

void OpeningAnimation::Play()
{
    timelineOpacity_->Start(1.0f);
}

void OpeningAnimation::ImGui()
{
    timelineOpacity_->ImGui("Opacity");
}
