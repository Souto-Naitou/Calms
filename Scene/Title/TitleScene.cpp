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

    pTextTitle_ = std::make_unique<Text>();
    pTextTitle_->Initialize();
    pTextTitle_->SetText("Shooting");
    pTextTitle_->SetMaxSize({ 1000, 300 });
    pTextTitle_->SetPosition({ 0, -150 });
    pTextTitle_->SetFontSize(99.9999f);
    pTextTitle_->SetColorName("White");
    pTextTitle_->SetAnchorPoint(TextStandardPoint::Center);
    pTextTitle_->SetPivot(TextStandardPoint::Center);
    pTextTitle_->SetName("Title");
    pTextTitle_->SetFontFamily("Bahnschrift");

    pTextStart_ = std::make_unique<Text>();
    pTextStart_->Initialize();
    pTextStart_->SetText("Press SPACE");
    pTextStart_->SetMaxSize({ 1000, 300 });
    pTextStart_->SetPosition({ 0, 200 });
    pTextStart_->SetFontSize(50.0f);
    pTextStart_->SetColorName("White");
    pTextStart_->SetAnchorPoint(TextStandardPoint::Center);
    pTextStart_->SetPivot(TextStandardPoint::Center);
    pTextStart_->SetName("Start");
    pTextStart_->SetFontFamily("Bahnschrift");
}

void TitleScene::Finalize()
{
    gameEye_.reset();

    pTextTitle_->Finalize();
    pTextStart_->Finalize();
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


    pTextTitle_->Update();
    pTextStart_->Update();
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
}

void TitleScene::DrawTexts()
{
    pTextTitle_->Draw();
    pTextStart_->Draw();
}
