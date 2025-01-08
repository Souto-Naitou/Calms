#include "ClearScene.h"

#include <Core/Win32/WinSystem.h>
#include <Features/Input/Input.h>

#include <Features/SceneTransition/SceneTransitionManager.h>
#include <Features/SceneTransition/TransFadeInOut.h>

void ClearScene::Initialize()
{
    pClear_ = new Sprite();
    pClear_->SetName("ClearSprite");
    pClear_->Initialize("clear.png");
    pClear_->SetAnchorPoint({ 0.5f, 0.5f });
    pClear_->SetPosition({ WinSystem::kClientWidth/2, WinSystem::kClientHeight/2});

    pSpace_ = new Sprite();
    pSpace_->SetName("SpaceSprite");
    pSpace_->Initialize("spacePress.png");
    pSpace_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpace_->SetPosition({ WinSystem::kClientWidth / 2, WinSystem::kClientHeight / 2 + 100 });
}

void ClearScene::Finalize()
{
    pClear_->Finalize();
    delete pClear_;

    pSpace_->Finalize();
    delete pSpace_;
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

void ClearScene::Draw2dBackGround()
{
    pClear_->Draw();
    pSpace_->Draw();
}

void ClearScene::Draw3d()
{
}

void ClearScene::Draw2dMidground()
{
}

void ClearScene::Draw3dMidground()
{
}

void ClearScene::DrawLine()
{
}

void ClearScene::Draw2dForeground()
{
}