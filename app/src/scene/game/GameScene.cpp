#include "GameScene.h"
#include <Features/event/EventListener.h>
#include <functional>
#include <config/ResourcePath.h>


void GameScene::Initialize()
{
    pInput_ = Input::GetInstance();
    auto pEventListener = EventListener::GetInstance();

    subscriptionPauseMenuToggle_ = pEventListener->Subscribe<PauseMenuToggleEvent>(
        std::bind(
            &GameScene::TogglePauseMenu, 
            this, 
            std::placeholders::_1
        )
    );

    /// [ 各レイヤーの初期化 ]
    if (!pPauseLayer_) pPauseLayer_ = std::make_unique<PauseLayer>();
    pPauseLayer_->Initialize(pArgs_, pLayer_);
    if (!pGameLayer_) pGameLayer_ = std::make_unique<GameLayer>();
    pGameLayer_->Initialize(pArgs_, pLayer_);

    auto effect = pGameLayer_->GetOverallCanvas()->GetPostEffectExecutor().AddEffect(PostEffectClassName::SeparatedGaussianFilter);
    pGaussianFilter_ = static_cast<SeparatedGaussianFilter*>(effect);
    {
        auto& optionGaussian = pGaussianFilter_->GetOption();
        optionGaussian.kernelSize = 15;
        pGaussianFilter_->SetSigma(0.0f);
        pGaussianFilter_->Enable(false);
    }

    /// [ BGMの初期化と再生 ]
    pBGM_ = AudioManager::GetInstance()->GetNewAudio("BGM", Path::Audio::kBgmInGame);
    pBGM_->SetVolume(0.1f);
    pBGM_->Play(true);
}

void GameScene::Finalize()
{
    pBGM_->Stop();
    pGameLayer_->Finalize();
    pPauseLayer_->Finalize();
}

void GameScene::Update()
{
    if (pInput_->TriggerKey(DIK_ESCAPE)) this->TogglePauseMenu();

    if (!isPauseMenuActive_) pGameLayer_->Update();
    
    pPauseLayer_->Update();

    /// [ BGMのフェードアウト ]
    if (isChangingScene_)
    {
        pBGM_->SetVolume(pBGM_->GetVolume() * 0.95f);
    }

    this->PauseBlurUpdate();
}

void GameScene::Draw()
{
    pGameLayer_->Draw();
    pPauseLayer_->Draw();
}

void GameScene::PreLoad(TaskExecutor& executor)
{
    PreloadContext ctx{};
    ctx.pSceneArgs = pArgs_;
    ctx.pLayer = pLayer_;
    pGameLayer_ = std::make_unique<GameLayer>();
    pGameLayer_->Preload(ctx, executor);
}

void GameScene::ImGui()
{
}


void GameScene::OnSceneChangeReserved()
{
    isChangingScene_ = true;
}

void GameScene::TogglePauseMenu(const PauseMenuToggleEvent&)
{
    isPauseMenuActive_ = !isPauseMenuActive_;
    pPauseLayer_->SetPaused(isPauseMenuActive_);
}

void GameScene::PauseBlurUpdate()
{
    constexpr float kMinFloat = 0.1f;
    float sigma = pGaussianFilter_->GetSigma();
    if (isPauseMenuActive_)
    {
        pGaussianFilter_->Enable(true);
        sigma = std::lerp(sigma, kPauseBlurSigmaMax_, kPauseBlurSigmaLerpFactorIncrease_);
    }
    else if (sigma > kMinFloat)
    {
        sigma = std::lerp(sigma, 0.0f, kPauseBlurSigmaLerpFactorDecrease_);
    }

    if (sigma <= kMinFloat)
    {
        sigma = 0.0f;
        pGaussianFilter_->Enable(false);
    }
    pGaussianFilter_->SetSigma(sigma);
}

