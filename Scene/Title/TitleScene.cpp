#include "TitleScene.h"
#include <Features/SceneTransition/TransFadeInOut.h>
#include <Core/Win32/WinSystem.h>

void TitleScene::Initialize()
{
    /// インスタンスの取得
    pInput_ = Input::GetInstance();
    pSceneTransition_ = SceneTransitionManager::GetInstance();

    /// ゲームアイの初期化
    gameEye_ = std::make_unique<GameEye>();
    gameEye_->SetName("main");
    gameEye_->SetTranslate(Vector3(0, 15.0f, -30.0f));
    gameEye_->SetRotate(Vector3(-1.2f, 0, 0));

    pSpace_ = new Sprite();
    pSpace_->SetName("SpaceSprite");
    pSpace_->Initialize("clear.png");
    pSpace_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpace_->SetPosition({ WinSystem::kClientWidth / 2, WinSystem::kClientHeight / 2 + 100 });
}

void TitleScene::Finalize()
{
    gameEye_.reset();

    pSpace_->Finalize();
    delete pSpace_;
}

void TitleScene::Update()
{
    gameEye_->Update();

    if (pInput_->TriggerKey(DIK_RETURN))
    {
        pSceneTransition_->ChangeScene("LobbyScene", std::make_unique<TransFadeInOut>());
    }
    else if (pInput_->TriggerKey(DIK_SPACE))
    {
        pSceneTransition_->ChangeScene("GameScene", std::make_unique<TransFadeInOut>());
    }

    pSpace_->Update();
}

void TitleScene::Draw2dBackGround()
{
}

void TitleScene::Draw3d()
{
}

void TitleScene::Draw2dMidground()
{
}

void TitleScene::Draw3dMidground()
{
}

void TitleScene::DrawLine()
{
}

void TitleScene::Draw2dForeground()
{
    pSpace_->Draw();
}
