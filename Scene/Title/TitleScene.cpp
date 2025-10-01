#include "TitleScene.h"
#include <Features/SceneTransition/TransFadeInOut.h>
#include <Features/Sprite/SpriteSystem.h>
#include <Features/Object3d/Object3dSystem.h>
#include <Features/Line/LineSystem.h>
#include <any>
#include <Core/DirectX12/TextureManager.h>

void TitleScene::Initialize()
{
    /// インスタンスの取得
    pInput_ = Input::GetInstance();
    pSceneTransition_ = SceneTransitionManager::GetInstance();
    pCubemapSystem_ = std::any_cast<CubemapSystem*>(pArgs_->Get("CubemapSystem"));

    /// ゲームアイの初期化
    gameEye_ = std::make_unique<GameEye>();
    gameEye_->SetName("main");
    gameEye_->SetTranslate(Vector3(0, 15.0f, -30.0f));
    gameEye_->SetRotate(Vector3(-1.2f, 0, 0));

    /// ゲームアイをセット
    Object3dSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    SpriteSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    LineSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    pCubemapSystem_->SetGlobalEye(gameEye_.get());

    // テキストの初期化
    this->InitializeTexts();

    // スプライトの初期化
    this->InitializeSprites();

    auto pTM = TextureManager::GetInstance();
    pTM->LoadTexture("Title/Skybox.dds");

    pSkybox_ = std::make_unique<Skybox>();
    pSkybox_->Initialize(pCubemapSystem_);
    pSkybox_->SetSkyboxTexture(pTM->GetSrvHandleGPU("Title/Skybox.dds"));
}

void TitleScene::Finalize()
{
    gameEye_.reset();

    pTextTitle_->Finalize();
    pTextStart_->Finalize();
}

void TitleScene::Update()
{
    Vector3 eyeRotate = gameEye_->GetTransform().rotate;
    eyeRotate.y += 0.001f;
    gameEye_->SetRotate(eyeRotate);
    gameEye_->Update();

    if (pInput_->TriggerKey(DIK_SPACE))
    {
        pSceneTransition_->ChangeScene("GameScene", std::make_unique<TransFadeInOut>());
    }

    pTextTitle_->Update();
    pTextStart_->Update();

    pSpriteBackground_->Update();
    pSpriteFilterImediate_->Update();

    pSkybox_->Update();
}

void TitleScene::Draw()
{
    //pSpriteBackground_->Draw();
    //pSpriteFilterImediate_->Draw();

    pCubemapSystem_->DrawSetting();
    pSkybox_->Draw();
}

void TitleScene::DrawTexts()
{
    pTextTitle_->Draw();
    pTextStart_->Draw();
}

void TitleScene::InitializeSprites()
{
    pSpriteBackground_ = std::make_unique<Sprite>();
    pSpriteBackground_->Initialize("Title/BG0.png");
    pSpriteBackground_->SetName("Background");
    pSpriteBackground_->SetPosition({ 0, 0 });
    pSpriteBackground_->SetSize({ 1600,900 });

    pSpriteFilterImediate_ = std::make_unique<Sprite>();
    pSpriteFilterImediate_->Initialize("white1x1.png");
    pSpriteFilterImediate_->SetName("FilterImediate");
    pSpriteFilterImediate_->SetPosition({ 0, 0 });
    pSpriteFilterImediate_->SetSize({ 1600,900 });
    pSpriteFilterImediate_->SetColor({ 0,0,0,0.5f });
}

void TitleScene::InitializeTexts()
{
    /// テキストの初期化
    pTextTitle_ = std::make_unique<Text>();
    pTextTitle_->Initialize();
    pTextTitle_->SetText("Calms");
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
