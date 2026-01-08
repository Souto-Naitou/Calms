#include "InputGuide.h"
#include <config/ResourcePath.h>
#include <Core/Win32/WinSystem.h>

void InputGuide::Initialize()
{
    const float margin = WinSystem::clientWidth / 40.0f;
    const float padding = WinSystem::clientWidth / 60.0f;
    float cursorY = WinSystem::clientHeight - margin;

    shoot_ = std::make_unique<Sprite>();
    shoot_->Initialize(Path::Image::kGuideShoot);
    shoot_->SetName("guide_shoot");
    shoot_->SetAnchorPoint({ 0.0f, 0.5f });
    cursorY -= shoot_->GetSize().y / 2.0f + padding;
    shoot_->SetPosition({ margin, cursorY });

    slomo_ = std::make_unique<Sprite>();
    slomo_->Initialize(Path::Image::kGuideSlow);
    slomo_->SetName("guide_slomo");
    slomo_->SetAnchorPoint({ 0.0f, 0.5f });
    cursorY -= (shoot_->GetSize().y / 2.0f) + (slomo_->GetSize().y / 2.0f) + padding;
    slomo_->SetPosition({ margin, cursorY });

    explosion_ = std::make_unique<Sprite>();
    explosion_->Initialize(Path::Image::kGuideExplosion);
    explosion_->SetName("guide_explosion");
    explosion_->SetAnchorPoint({ 0.0f, 0.5f });
    cursorY -= (slomo_->GetSize().y / 2.0f) + (explosion_->GetSize().y / 2.0f) + padding;
    explosion_->SetPosition({ margin, cursorY });
}

void InputGuide::Update()
{
    shoot_->Update();
    slomo_->Update();
    explosion_->Update();
}

void InputGuide::Draw1F()
{
    shoot_->Draw1F();
    slomo_->Draw1F();
    explosion_->Draw1F();
}

void InputGuide::Finalize()
{
    shoot_->Finalize();
    slomo_->Finalize();
    explosion_->Finalize();
}
