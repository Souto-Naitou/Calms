#include "TitleScene.h"
#include <Effects/SceneTransition/TransShutter.h>
#include <Features/Sprite/SpriteSystem.h>
#include <Features/Object3d/Object3dSystem.h>
#include <Features/Line/LineSystem.h>
#include <any>
#include <Core/DirectX12/TextureManager.h>
#include <config/ResourcePath.h>
#include <Color.h>
#include <cmath>
#include <DebugTools/Logger/Logger.h>

void TitleScene::Initialize()
{
    /// インスタンスの取得
    pInput_ = Input::GetInstance();
    pSceneTransition_ = SceneTransitionManager::GetInstance();
    pCubemapSystem_ = std::any_cast<CubemapSystem*>(pArgs_->Get("CubemapSystem"));
    pDx12_ = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));
    pPostEffectExecuter_ = std::any_cast<PostEffectExecuter*>(pArgs_->Get("PostEffectExecuter"));

    /// Canvasの初期化
    {
        CanvasInitParams params = {};
        params.name = "TitleCanvas";
        params.pDx12 = pDx12_;

        pCanvas_ = std::make_unique<Canvas>();
        pCanvas_->Initialize(params);
    }

    // ゲームアイの初期化
    this->InitializeGameEye();

    // スプライトの初期化
    this->InitializeSprites();

    // スカイボックスの初期化
    this->InitializeSkybox();

    /// ランダムフィルタの初期化と登録
    {
        auto tempPointer = pCanvas_->GetPostEffectExecuter().AddEffect(PostEffectClassName::RandomFilter);
        try
        {
            pRandomFilter_ = static_cast<RandomFilter*>(tempPointer);
        }
        catch (const std::exception& e)
        {
            Logger::GetInstance()->LogError(
                __FILE__,
                __FUNCTION__,
                std::string("Failed to convert to RandomFilter pointer: ") + e.what()
            );
        }
    }

    
    //pPostEffectExecuter_->RegisterPostEffect(pRandomFilter_.get());

    // オープニングアニメーションの初期化と再生
    // - 実時間をもとに再生されるためPlay関数のあとに時間のかかる処理(I/O など)を入れないこと
    pOpeningAnimation_ = std::make_unique<OpeningAnimation>();
    pOpeningAnimation_->Initialize();
    pOpeningAnimation_->Play();
}

void TitleScene::Finalize()
{
    gameEye_.reset();
    pPostEffectExecuter_->RemoveEffect(pRandomFilter_);
}

void TitleScene::Update()
{
    Vector3 eyeRotate = gameEye_->GetTransform().rotate;
    eyeRotate.y += 0.001f;
    gameEye_->SetRotate(eyeRotate);
    gameEye_->Update();

    if (pInput_->TriggerKey(DIK_SPACE))
    {
        pSceneTransition_->ChangeScene("LoadScene", std::make_unique<TransShutter>());
    }

    this->UpdateTitleAnimation();
    this->UpdateStartPromptAnimation();

    //pRandomFilter_->SetSeed(eyeRotate.y);

    pSkybox_->Update();
    pSpriteTitle_->Update();
    pSpriteFrameScreen_->Update();
    pSpritePressStart_->Update();
    pOpeningAnimation_->Update();
}

void TitleScene::Draw()
{
    pCubemapSystem_->DrawSetting();
    pSkybox_->Draw();

    pSpriteTitle_->Draw();
    pSpriteFrameScreen_->Draw();
    pSpritePressStart_->Draw();
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
    pSpriteTitle_->Initialize(Path::Image::kTitle);
    pSpriteTitle_->SetName("Title");
    pSpriteTitle_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpriteTitle_->SetPosition({ WinSystem::clientWidth / 2.0f, WinSystem::clientHeight / 2.0f - 50.0f });

    /// フレームスクリーンの初期化
    pSpriteFrameScreen_ = std::make_unique<Sprite>();
    pSpriteFrameScreen_->Initialize(Path::Image::kFrameScreen);
    pSpriteFrameScreen_->SetName("FrameScreen");
    pSpriteFrameScreen_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpriteFrameScreen_->SetPosition({ WinSystem::clientWidth / 2.0f, WinSystem::clientHeight / 2.0f });
    pSpriteFrameScreen_->SetSize({ static_cast<float>(WinSystem::clientWidth), static_cast<float>(WinSystem::clientHeight) });
    pSpriteFrameScreen_->SetColor(RGBA(0x101010ff).to_Vector4());

    /// 開始プロンプトの初期化
    pSpritePressStart_ = std::make_unique<Sprite>();
    pSpritePressStart_->Initialize(Path::Image::kTitleStartPrompt);
    pSpritePressStart_->SetName("PressStart");
    pSpritePressStart_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpritePressStart_->SetPosition({ WinSystem::clientWidth / 2.0f, WinSystem::clientHeight / 2.0f + 200.0f });
    pSpritePressStart_->SetSizeWithFactor(1.05f);
}

void TitleScene::InitializeSkybox()
{
    auto pTM = TextureManager::GetInstance();
    pTM->LoadTexture(Path::Image::kTitleSkybox);

    pSkybox_ = std::make_unique<Skybox>();
    pSkybox_->Initialize(pCubemapSystem_);
    pSkybox_->SetSkyboxTexture(pTM->GetSrvHandleGPU(Path::Image::kTitleSkybox));
}

void TitleScene::UpdateTitleAnimation()
{
    static float t = 0.0f;
    float x = pSpriteTitle_->GetPosition().x;
    float y = kPosYTitle_ + std::sinf(t) * 5.0f;
    pSpriteTitle_->SetPosition(Vector2(x, y));
    t += 0.04f;
}

void TitleScene::UpdateStartPromptAnimation()
{
    // FIX: 別クラスを立ててUIアニメーションを管理する
    static float t = 0.0f;
    opacityStartPrompt_ = (std::sinf(t) + 1.5f) / 3.0f;
    t += 0.04f;
    pSpritePressStart_->SetColor(Vector4(1.0f, 1.0f, 1.0f, opacityStartPrompt_));
}
