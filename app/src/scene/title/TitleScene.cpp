#include "TitleScene.h"
#include <Effects/SceneTransition/TransShutter.h>
#include <drawable/sprite/SpriteSystem.h>
#include <drawable/object3d/Object3dSystem.h>
#include <Features/Line/LineSystem.h>
#include <any>
#include <Core/DirectX12/TextureManager.h>
#include <config/ResourcePath.h>
#include <Color.h>
#include <cmath>
#include <Features/Audio/AudioManager.h>
#include <Features/Layer/CanvasScope.h>
#include <Math/ViewportUnits.hpp>

void TitleScene::Initialize()
{
    /// 繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ縺ｮ蜿門ｾ・    pInput_ = Input::GetInstance();
    pSceneManager_ = SceneManager::GetInstance();
    pCubemapSystem_ = std::any_cast<CubemapSystem*>(pArgs_->Get("CubemapSystem"));
    pDx12_ = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));

    /// Canvas縺ｮ蛻晄悄蛹・    {
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

    // 繧ｲ繝ｼ繝繧｢繧､縺ｮ蛻晄悄蛹・    this->InitializeGameEye();

    // 繧ｹ繧ｫ繧､繝懊ャ繧ｯ繧ｹ縺ｮ蛻晄悄蛹・    this->InitializeSkybox();

    // 繧ｹ繝励Λ繧､繝医・蛻晄悄蛹・    this->InitializeSprites();

    /// 繝輔ぅ繝ｫ繧ｿ縺ｮ蛻晄悄蛹悶→逋ｻ骭ｲ
    {
        auto tempBloom = pCanvasBack_->GetPostEffectExecutor().AddEffect(PostEffectClassName::GaussianBloom);
        auto tempRandom = pCanvasBack_->GetPostEffectExecutor().AddEffect(PostEffectClassName::RandomFilter);
        auto tempRadial = pCanvasSprite_->GetPostEffectExecutor().AddEffect(PostEffectClassName::RadialBlur);
        pRandomFilter_ = static_cast<RandomFilter*>(tempRandom);
        pGaussianBloom_ = static_cast<GaussianBloom*>(tempBloom);
        pRadialBlur_ = static_cast<RadialBlur*>(tempRadial);
    }

    this->InitializePostEffects();

    pSoundStartButton_ = AudioManager::GetInstance()->GetNewAudio("Effect", Path::Audio::kSeStartButton);
    pSoundStartButton_->SetVolume(0.1f);

    pSoundBGM_ = AudioManager::GetInstance()->GetNewAudio("BGM", Path::Audio::kBgmTitle);
    pSoundBGM_->SetVolume(0.075f);
    pSoundBGM_->Play(true);

    // 繧ｪ繝ｼ繝励ル繝ｳ繧ｰ繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ縺ｮ蛻晄悄蛹悶→蜀咲函
    // - 螳滓凾髢薙ｒ繧ゅ→縺ｫ蜀咲函縺輔ｌ繧九◆繧￣lay髢｢謨ｰ縺ｮ縺ゅ→縺ｫ譎る俣縺ｮ縺九°繧句・逅・I/O 縺ｪ縺ｩ)繧貞・繧後↑縺・％縺ｨ
    pOpeningAnimation_ = std::make_unique<OpeningAnimation>();
    pOpeningAnimation_->Initialize();
    pOpeningAnimation_->Play();
}

void TitleScene::Finalize()
{
    pSoundBGM_->Stop();
    gameEye_.reset();
    pCanvasBack_->GetPostEffectExecutor().RemoveEffect(pRandomFilter_);

    pLayer_->RemoveCanvas(pCanvasBack_.get());
    pLayer_->RemoveCanvas(pCanvasSprite_.get());
    pCanvasBack_->Finalize();
    pCanvasSprite_->Finalize();
}

void TitleScene::Update()
{
    Vector3 eyeRotate = gameEye_->GetTransform().rotate;
    eyeRotate.y += 0.001f;
    gameEye_->SetRotate(eyeRotate);
    gameEye_->Update();

    if (pInput_->TriggerKey(DIK_SPACE) && !isChangingScene_)
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

    pRandomFilter_->SetSeed(eyeRotate.y);

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
    /// 繧ｲ繝ｼ繝繧｢繧､縺ｮ蛻晄悄蛹・    gameEye_ = std::make_unique<GameEye>();
    gameEye_->SetName("main");
    gameEye_->SetTranslate(Vector3(0, 15.0f, -30.0f));
    gameEye_->SetRotate(Vector3(-1.2f, 0, 0));

    /// 繧ｲ繝ｼ繝繧｢繧､繧偵そ繝・ヨ
    Object3dSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    SpriteSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    LineSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    pCubemapSystem_->SetGlobalEye(gameEye_.get());
}

void TitleScene::InitializeSprites()
{
    /// 繧ｿ繧､繝医Ν繝・く繧ｹ繝医・蛻晄悄蛹・    pSpriteTitle_ = std::make_unique<Sprite>();
    pSpriteTitle_->Initialize(Path::Image::kTitle);
    pSpriteTitle_->SetName("Title");
    pSpriteTitle_->SetAnchorPoint({ 0.5f, 0.5f });

    pSpriteTitle_->SetPosition({ 50.0_vw, 50.0_vh - 50.0f});

    /// 繝輔Ξ繝ｼ繝繧ｹ繧ｯ繝ｪ繝ｼ繝ｳ縺ｮ蛻晄悄蛹・    pSpriteFrameScreen_ = std::make_unique<Sprite>();
    pSpriteFrameScreen_->Initialize(Path::Image::kFrameScreen);
    pSpriteFrameScreen_->SetName("FrameScreen");
    pSpriteFrameScreen_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpriteFrameScreen_->SetPosition({ 50.0_vw, 50.0_vh });
    pSpriteFrameScreen_->SetSize({ 100.0_vw, 100.0_vh });
    pSpriteFrameScreen_->SetColor(RGBA(0x101010ff).to_Vector4());

    /// 髢句ｧ九・繝ｭ繝ｳ繝励ヨ縺ｮ蛻晄悄蛹・    pSpritePressStart_ = std::make_unique<Sprite>();
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
    pRandomFilter_->Enable(true);
    pGaussianBloom_->Enable(true);
    pRadialBlur_->Enable(true);

    pRandomFilter_->SetOpacity(0.15f);
    pGaussianBloom_->SetKernelSize(31);
    pGaussianBloom_->SetSigma(27.9f);
    pGaussianBloom_->SetThreshold(0.313f);
    pGaussianBloom_->SetBloomIntensity(2.14f);
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
    // FIX: 蛻･繧ｯ繝ｩ繧ｹ繧堤ｫ九※縺ｦUI繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ繧堤ｮ｡逅・☆繧・    static float t = 0.0f;
    opacityStartPrompt_ = (std::sinf(t) + 1.5f) / 3.0f;
    t += 0.04f;
    pSpritePressStart_->SetColor(Vector4(1.0f, 1.0f, 1.0f, opacityStartPrompt_));
}
