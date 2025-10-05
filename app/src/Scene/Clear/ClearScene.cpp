#include "ClearScene.h"

#include <Core/Win32/WinSystem.h>
#include <Features/Input/Input.h>

#include <Features/SceneTransition/SceneTransitionManager.h>
#include <Features/SceneTransition/TransFadeInOut.h>

void ClearScene::Initialize()
{
    pSpace_ = std::make_unique<Sprite>();
    pSpace_->Initialize("spacePress.png");
    pSpace_->SetName("SpaceSprite");
    pSpace_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpace_->SetPosition({ WinSystem::clientWidth / 2, WinSystem::clientHeight / 2 + 100 });

    pClear_ = std::make_unique<Sprite>();
    pClear_->Initialize("clear.png");
    pClear_->SetName("ClearSprite");
    pClear_->SetAnchorPoint({ 0.5f, 0.5f });
    pClear_->SetPosition({ WinSystem::clientWidth / 2, WinSystem::clientHeight / 2 });
}

void ClearScene::Finalize()
{
    pClear_->Finalize();
    pSpace_->Finalize();
}

void ClearScene::Update()
{
    pClear_->Update();
    pSpace_->Update();

    if (Input::GetInstance()->TriggerKey(DIK_SPACE))
    {
        SceneTransitionManager::GetInstance()->ChangeScene("TitleScene", std::make_unique<TransFadeInOut>());
    }
}

void ClearScene::Draw()
{
    pClear_->Draw();
    pSpace_->Draw();
}

void ClearScene::DrawTexts()
{
}
