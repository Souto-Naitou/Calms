#include "TitleScene.h"
#include <Effects/SceneTransition/TransShutter.h>
#include <drawable/sprite/SpriteSystem.h>
#include <drawable/object3d/Object3dSystem.h>
#include <drawable/line/LineSystem.h>
#include <any>
#include <Core/DirectX12/TextureManager.h>
#include <config/ResourcePath.h>
#include <Color.h>
#include <cmath>
#include <Features/Audio/AudioManager.h>
#include <Features/Layer/CanvasScope.h>
#include <Math/ViewportUnits.hpp>
#include <NiGui.h>
#include <Math/Easing.h>

void TitleScene::Initialize()
{
    /// インスタンスの取得
    pInput_ = Input::GetInstance();
    pSceneManager_ = SceneManager::GetInstance();
    pCubemapSystem_ = std::any_cast<CubemapSystem*>(pArgs_->Get("CubemapSystem"));
    pDx12_ = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));

    /// Canvasの初期化
    {
        Canvas::Params params = {};
        params.name = "TitleCanvas";
        params.pDx12 = pDx12_;
        params.pCubemapSystem = pCubemapSystem_;
        #ifdef _DEBUG
        params.pImGuiManager = std::any_cast<ImGuiManager*>(pArgs_->Get("ImGuiManager"));
        #endif // _DEBUG

        pCanvasBack_ = std::make_unique<Canvas>();
        pCanvasBack_->Initialize(params);

        params.name = "TitleCanvas2";
        pCanvasSprite_ = std::make_unique<Canvas>();
        pCanvasSprite_->Initialize(params);

        pLayer_->AddCanvas(pCanvasBack_.get());
        pLayer_->AddCanvas(pCanvasSprite_.get());
    }

    // ゲームアイの初期化
    this->InitializeGameEye();

    // スカイボックスの初期化
    this->InitializeSkybox();

    // スプライトの初期化
    this->InitializeSprites();

    /// フィルタの初期化と登録
    {
        auto tempBloom = pCanvasBack_->GetPostEffectExecutor().AddEffect(PostEffectClassName::GaussianBloom);
        auto tempGaussian = pCanvasBack_->GetPostEffectExecutor().AddEffect(PostEffectClassName::SeparatedGaussianFilter);
        auto tempMosaic = pCanvasBack_->GetPostEffectExecutor().AddEffect(PostEffectClassName::Mosaic);
        auto tempRadial = pCanvasSprite_->GetPostEffectExecutor().AddEffect(PostEffectClassName::RadialBlur);
        pGaussianBloom_ = static_cast<GaussianBloom*>(tempBloom);
        pMosaic_ = static_cast<Mosaic*>(tempMosaic);
        pSeparatedGaussianFilter_ = static_cast<SeparatedGaussianFilter*>(tempGaussian);
    }

    this->InitializePostEffects();

    pSoundStartButton_ = AudioManager::GetInstance()->GetNewAudio("Effect", Path::Audio::kSeStartButton);
    pSoundStartButton_->SetVolume(0.1f);

    pSoundBGM_ = AudioManager::GetInstance()->GetNewAudio("BGM", Path::Audio::kBgmTitle);
    pSoundBGM_->SetVolume(0.075f);
    pSoundBGM_->Play(true);

    // オープニングアニメーションの初期化と再生
    // - 実時間をもとに再生されるためPlay関数のあとに時間のかかる処理(I/O など)を入れないこと
    pOpeningAnimation_ = std::make_unique<OpeningAnimation>();
    pOpeningAnimation_->Initialize();
    pOpeningAnimation_->Play();
}

void TitleScene::Finalize()
{
    pSoundBGM_->Stop();
    gameEye_.reset();
    pLayer_->RemoveCanvas(pCanvasBack_.get());
    pLayer_->RemoveCanvas(pCanvasSprite_.get());
    pCanvasBack_->Finalize();
    pCanvasSprite_->Finalize();
}

void TitleScene::Update()
{
    Vector3 eyeRotate = gameEye_->GetTransform().rotate;
    eyeRotate.y += 0.001f;

    float t = (std::sinf(eyeRotate.y * 10.0f) + 1.0f) / 2.0f; // 0から1の範囲で変化する値
    Vector3 eyePos = gameEye_->GetTransform().translate;
    eyePos.z = std::lerp(kEyePosZMin_, kEyePosZMax_, Math::Easing::EaseInOutSine(t));
    gameEye_->SetRotate(eyeRotate);
    gameEye_->SetTranslate(eyePos);
    gameEye_->Update();

    float threshold = std::lerp(kBloomThresholdMin_, 0.5f, Math::Easing::EaseInOutSine(t));
    pGaussianBloom_->SetThreshold(threshold);

    t = (std::sinf(eyeRotate.y * 20.0f) + 1.0f) / 2.0f; // 0から1の範囲で変化する値
    float kernelSize = std::lerp(3.0f, 31.0f, Math::Easing::EaseInOutQuad(t));
    pSeparatedGaussianFilter_->GetOption().kernelSize = static_cast<int>(kernelSize);
    pSeparatedGaussianFilter_->CreateKernel();

    if (pInput_->ReleaseKey(DIK_SPACE) && !isChangingScene_)
    {
        pSoundStartButton_->Play();
        pTransShutter_ = std::make_unique<TransShutter>();
        pSceneManager_->ReserveScene("GameScene", "LoadingScreen", std::move(pTransShutter_));
        isChangingScene_ = true;
    }

    if (isChangingScene_)
    {
        pSoundBGM_->SetVolume(pSoundBGM_->GetVolume() * 0.95f);
    }

    this->UpdateTitleAnimation();
    this->UpdateStartPromptAnimation();

    pSkybox_->Update();
    pSpriteTitle_->Update();
    pSpriteFrameScreen_->Update();
    pSpritePressStart_->Update();
    pOpeningAnimation_->Update();
}

void TitleScene::Draw()
{
    CanvasScope canvasScopeBack(pCanvasSprite_.get());
    pSpriteFrameScreen_->Draw1F();
    pSpriteTitle_->Draw1F();
    pSpritePressStart_->Draw1F();
    pOpeningAnimation_->Draw1F();
}

void TitleScene::InitializeGameEye()
{
    /// ゲームアイの初期化
    gameEye_ = std::make_unique<GameEye>();
    gameEye_->SetName("main");
    gameEye_->SetTranslate(Vector3(0, 15.0f, -30.0f));
    gameEye_->SetRotate(Vector3(-1.2f, 0, 0));
    gameEye_->SetFov(1.2f);

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

    pSpriteTitle_->SetPosition({ 50.0_vw, 50.0_vh - 50.0f});

    /// フレームスクリーンの初期化
    pSpriteFrameScreen_ = std::make_unique<Sprite>();
    pSpriteFrameScreen_->Initialize(Path::Image::kFrameScreen);
    pSpriteFrameScreen_->SetName("FrameScreen");
    pSpriteFrameScreen_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpriteFrameScreen_->SetPosition({ 50.0_vw, 50.0_vh });
    pSpriteFrameScreen_->SetSize({ 100.0_vw, 100.0_vh });
    pSpriteFrameScreen_->SetColor(RGBA(0x101010ff).to_Vector4());

    /// 開始プロンプトの初期化
    pSpritePressStart_ = std::make_unique<Sprite>();
    pSpritePressStart_->Initialize(Path::Image::kTitleStartPrompt);
    pSpritePressStart_->SetName("PressStart");
    pSpritePressStart_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpritePressStart_->SetPosition({ 50.0_vw, 50.0_vh + 200.0f });
    pSpritePressStart_->SetSizeWithFactor(1.05f);
}

void TitleScene::InitializeSkybox()
{
    auto pTM = TextureManager::GetInstance();
    pTM->LoadTexture(Path::Image::kTitleSkybox);

    pSkybox_ = std::make_unique<Skybox>();
    pSkybox_->Initialize(pCubemapSystem_);
    pSkybox_->SetSkyboxTexture(pTM->GetSrvHandleGPU(Path::Image::kTitleSkybox));

    pCanvasBack_->RegisterDrawable(pSkybox_.get());
}

void TitleScene::InitializePostEffects()
{
    pGaussianBloom_->Enable(true);
    pSeparatedGaussianFilter_->Enable(true);
    pMosaic_->Enable(true);

    pGaussianBloom_->SetKernelSize(31);
    pGaussianBloom_->SetSigma(27.9f);
    pGaussianBloom_->SetThreshold(0.313f);
    pGaussianBloom_->SetBloomIntensity(2.14f);

    pSeparatedGaussianFilter_->SetSigma(27.0f);

    pMosaic_->GetOption().power = 200.0f;
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

    if (pInput_->PushKey(DIK_SPACE))
    {
        pSpritePressStart_->SetSizeWithFactor(kPressSpaceScaleActive_);
    }
    else
    {
        pSpritePressStart_->SetSizeWithFactor(1.05f);
    }
}
