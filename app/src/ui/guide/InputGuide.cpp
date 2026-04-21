#include "InputGuide.h"
#include <config/ResourcePath.h>
#include <Core/Window/Window.h>
#include <Math/ViewportUnits.hpp>
#include <Core/DirectX12/TextureManager.h>

using namespace Math::Viewport::Unit;

void InputGuide::Initialize()
{
    const float margin = Window::clientWidth / 40.0f;
    const float padding = Window::clientWidth / 60.0f;
    float cursorY = Window::clientHeight - margin;

    /// ポーズ
    pause_ = std::make_unique<Sprite>();
    pause_->Initialize(Path::Image::InGame::kGuidePause);
    pause_->SetName("guide_pause");
    pause_->SetAnchorPoint({ 0.0f, 0.5f });
    pause_->SetPosition({ margin, 10_vh });

    /// スロー移動
    slomo_ = std::make_unique<Sprite>();
    slomo_->Initialize(Path::Image::InGame::kGuideSlow);
    slomo_->SetName("guide_slomo");
    slomo_->SetAnchorPoint({ 0.0f, 0.5f });
    cursorY -= slomo_->GetSize().y / 2.0f;
    slomo_->SetPosition({ margin, cursorY });
    cursorY -= slomo_->GetSize().y / 2.0f + padding;

    /// 爆発
    explosion_ = std::make_unique<Sprite>();
    explosion_->Initialize(Path::Image::InGame::kGuideExplosion);
    explosion_->SetName("guide_explosion");
    explosion_->SetAnchorPoint({ 0.0f, 0.5f });
    cursorY -= explosion_->GetSize().y / 2.0f;
    explosion_->SetPosition({ margin, cursorY });
    cursorY -= explosion_->GetSize().y / 2.0f + padding;

    /// シュート
    shoot_ = std::make_unique<Sprite>();
    shoot_->Initialize(Path::Image::InGame::kGuideShoot);
    shoot_->SetName("guide_shoot");
    shoot_->SetAnchorPoint({ 0.0f, 0.5f });
    cursorY -= shoot_->GetSize().y / 2.0f;
    shoot_->SetPosition({ margin, cursorY });
    cursorY -= shoot_->GetSize().y / 2.0f + padding;

    this->InitializeInputAwareSprite();
}

void InputGuide::Update()
{
    pause_->Update();
    shoot_->Update();
    slomo_->Update();
    explosion_->Update();
}

void InputGuide::Draw1F()
{
    pause_->Draw1F();
    shoot_->Draw1F();
    slomo_->Draw1F();
    explosion_->Draw1F();
}

void InputGuide::Finalize()
{
    pause_->Finalize();
    shoot_->Finalize();
    slomo_->Finalize();
    explosion_->Finalize();
}

void InputGuide::InitializeInputAwareSprite()
{
    const float margin = Window::clientWidth / 40.0f;
    const float padding = Window::clientWidth / 60.0f;
    float cursorY = Window::clientHeight - margin;

    inputAwareSprite_.Initialize();

    auto tm = TextureManager::GetInstance();
    DirectX::TexMetadata metadata = {};

    InputAwareSprite::Entry pauseEntry = {};
    pauseEntry.pSprite_ = pause_.get();
    pauseEntry.handleKeyboard_ = tm->GetSrvHandleGPU(Path::Image::InGame::kGuidePause);
    pauseEntry.handleGamepad_ = tm->GetSrvHandleGPU(Path::Image::InGame::GamePad::kGuidePause);
    metadata = tm->GetMetaData(Path::Image::InGame::GamePad::kGuidePause);
    pauseEntry.positionGamepad_ = { margin, 10_vh };
    inputAwareSprite_.AddEntry(pauseEntry);

    /// スロモ
    InputAwareSprite::Entry slomoEntry = {};
    slomoEntry.pSprite_ = slomo_.get();
    slomoEntry.handleKeyboard_ = tm->GetSrvHandleGPU(Path::Image::InGame::kGuideSlow);
    slomoEntry.handleGamepad_ = tm->GetSrvHandleGPU(Path::Image::InGame::GamePad::kGuideSlow);
    slomoEntry.positionKeyboard_ = slomo_->GetPosition();
    metadata = tm->GetMetaData(Path::Image::InGame::GamePad::kGuideSlow);
    cursorY -= static_cast<float>(metadata.height / 2);
    slomoEntry.positionGamepad_ = { margin, cursorY };
    cursorY -= static_cast<float>(metadata.height / 2 + padding);
    inputAwareSprite_.AddEntry(slomoEntry);

    InputAwareSprite::Entry explosionEntry = {};
    explosionEntry.pSprite_ = explosion_.get();
    explosionEntry.handleKeyboard_ = tm->GetSrvHandleGPU(Path::Image::InGame::kGuideExplosion);
    explosionEntry.handleGamepad_ = tm->GetSrvHandleGPU(Path::Image::InGame::GamePad::kGuideExplosion);
    explosionEntry.positionKeyboard_ = explosion_->GetPosition();
    metadata = tm->GetMetaData(Path::Image::InGame::GamePad::kGuideExplosion);
    cursorY -= static_cast<float>(metadata.height / 2);
    explosionEntry.positionGamepad_ = { margin, cursorY };
    cursorY -= static_cast<float>(metadata.height / 2 + padding);
    inputAwareSprite_.AddEntry(explosionEntry);

    InputAwareSprite::Entry shootEntry = {};
    shootEntry.pSprite_ = shoot_.get();
    shootEntry.handleKeyboard_ = tm->GetSrvHandleGPU(Path::Image::InGame::kGuideShoot);
    shootEntry.handleGamepad_ = tm->GetSrvHandleGPU(Path::Image::InGame::GamePad::kGuideShoot);
    shootEntry.positionKeyboard_ = shoot_->GetPosition();
    metadata = tm->GetMetaData(Path::Image::InGame::GamePad::kGuideShoot);
    cursorY -= static_cast<float>(metadata.height / 2);
    shootEntry.positionGamepad_ = { margin, cursorY };
    cursorY -= static_cast<float>(metadata.height / 2 + padding);
    inputAwareSprite_.AddEntry(shootEntry);

    inputAwareSprite_.ApplyCurrentMode();
}
