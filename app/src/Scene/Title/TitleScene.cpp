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

    // ゲームアイの初期化
    this->InitializeGameEye();

    // テキストの初期化
    this->InitializeSprites();

    // スカイボックスの初期化
    this->InitializeSkybox();


    // オープニングアニメーションの初期化と再生
    // - 実時間をもとに再生されるためPlay関数のあとに時間のかかる処理(I/O など)を入れないこと
    pOpeningAnimation_ = std::make_unique<OpeningAnimation>();
    pOpeningAnimation_->Initialize();
    pOpeningAnimation_->Play();
}

void TitleScene::Finalize()
{
    gameEye_.reset();
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

    pSkybox_->Update();

    pOpeningAnimation_->Update();
}

void TitleScene::Draw()
{
    pCubemapSystem_->DrawSetting();
    pSkybox_->Draw();

    pOpeningAnimation_->Draw2d();
}

void TitleScene::DrawTexts()
{
}

void TitleScene::InitializeGameEye()
{
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
}

void TitleScene::InitializeSprites()
{
    /// タイトルテキストの初期化
    pSpriteTitle_ = std::make_unique<Sprite>();
    pSpriteTitle_->Initialize("Title/Title.png");
}

void TitleScene::InitializeSkybox()
{
    auto pTM = TextureManager::GetInstance();
    pTM->LoadTexture("Title/Skybox.dds");

    pSkybox_ = std::make_unique<Skybox>();
    pSkybox_->Initialize(pCubemapSystem_);
    pSkybox_->SetSkyboxTexture(pTM->GetSrvHandleGPU("Title/Skybox.dds"));
}
