#include "GameScene.h"
#include <Features/event/EventListener.h>
#include <functional>
#include <config/ResourcePath.h>
#include <any>


GameScene::GameScene(ISceneArgs* pArgs) : ILoadableScene(pArgs)
{
    pInputMapperUI_ = std::any_cast<InputMapper<InputActionUI>*>(pArgs_->Get("InputMapperUI"));
}

void GameScene::Initialize()
{
    auto pEventListener = EventListener::GetInstance();

    subscriptionPauseMenuToggle_ = pEventListener->Subscribe<PauseMenuToggleEvent>(
        std::bind(
            &GameScene::TogglePauseMenu, 
            this, 
            std::placeholders::_1
        )
    );
   
    /// [ ライトの初期化 ]
    DirectX12* pDx12 = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));
    pDirectionalLight_ = std::make_unique<DirectionalLight>(pDx12->GetDevice());
    pDirectionalLight_->Initialize();
    pPointLight_ = std::make_unique<PointLight>(pDx12->GetDevice());
    pPointLight_->Initialize();

    /// [ デフォルトで使用する光源を登録 ]
    Object3dSystem::GetInstance()->SetDirectionalLight(pDirectionalLight_.get());
    Object3dSystem::GetInstance()->SetPointLight(pPointLight_.get());
    Object3dInstancedSystem::GetInstance()->SetDirectionalLight(pDirectionalLight_.get());
    Object3dInstancedSystem::GetInstance()->SetPointLight(pPointLight_.get());

    /// [ シーンからレイヤーに渡したいデータはここでSceneArgsに追加 ]
    pArgs_->Set("DirectionalLight", pDirectionalLight_.get());
    pArgs_->Set("PointLight", pPointLight_.get());

    /// [ 各レイヤーの初期化 ]
    if (!pPauseLayer_) pPauseLayer_ = std::make_unique<PauseLayer>();
    pPauseLayer_->Initialize(pArgs_, pLayer_);
    if (!pGameLayer_) pGameLayer_ = std::make_unique<GameLayer>();
    pGameLayer_->Initialize(pArgs_, pLayer_);

    /// [ ポーズ用のブラーエフェクトの初期化 ]
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
    if (pInputMapperUI_->IsTrigger(InputActionUI::Pause)) this->TogglePauseMenu();

    if (!isPauseMenuActive_) pGameLayer_->Update();
    pPauseLayer_->Update();

    /// [ ライトの更新 ]
    pDirectionalLight_->Update();
    pPointLight_->Update();

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

