#include "LoadScene.h"
#include "config/ResourcePath.h"
#include <Core/Win32/WinSystem.h>
#include <filesystem>
#include <Core/ConfigManager/ConfigManager.h>
#include <cctype>
#include <vector>
#include <Effects/SceneTransition/TransShutter.h>
#include <Features/SceneManager/SceneManager.h>
#include <Features/Layer/CanvasScope.h>


void LoadScene::Initialize()
{
    pInput_ = Input::GetInstance();

    DirectX12* pDx12 = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));
    CubemapSystem* pCubemapSystem = std::any_cast<CubemapSystem*>(pArgs_->Get("CubemapSystem"));

    Canvas::Params canvasParams = {};
    canvasParams.name = "LoadSceneCanvas";
    canvasParams.pDx12 = pDx12;
    canvasParams.pCubemapSystem = pCubemapSystem;
    #ifdef _DEBUG
    canvasParams.pImGuiManager = std::any_cast<ImGuiManager*>(pArgs_->Get("ImGuiManager"));
    #endif // _DEBUG
    pCanvas_ = std::make_unique<Canvas>();
    pCanvas_->Initialize(canvasParams);
    pLayer_->AddCanvas(pCanvas_.get());
    
    /// ローディングスプライトの初期化
    pTextureManager_ = TextureManager::GetInstance();
    pSpriteLoading_ = std::make_unique<Sprite>();
    pSpriteLoading_->Initialize(Path::Image::kLoading);
    pSpriteLoading_->SetName("Loading");
    pSpriteLoading_->SetAnchorPoint({ 1.0f, 0.5f });
    pSpriteLoading_->SetPosition({ WinSystem::clientWidth - WinSystem::clientWidth / 12.8f, WinSystem::clientHeight - 80.0f});
    pSpriteLoading_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
    pSpriteLoading_->SetSizeWithFactor(WinSystem::clientWidth / 1600.0f);

    /// ローディング背景スプライトの初期化
    pSpriteLBackground_ = std::make_unique<Sprite>();
    pSpriteLBackground_->Initialize("white1x1.png");
    pSpriteLBackground_->SetName("LoadingBackground");
    pSpriteLBackground_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f });
    pSpriteLBackground_->SetSize({ WinSystem::clientWidth, WinSystem::clientHeight });

    /// ローディングバーの初期化
    Bar2dInitParams barParams = {};
    barParams.barSize = { WinSystem::clientWidth / 2.0f, WinSystem::clientHeight / 30.0f };
    pBar_ = std::make_unique<Bar2d>();
    pBar_->Initialize(barParams);
    pBar_->SetAnchorPoint({ 1.0f, 0.5f });
    pBar_->SetPosition({ WinSystem::clientWidth - WinSystem::clientWidth / 2.5f, WinSystem::clientHeight - 80.0f });
    pBar_->SetCurrentValue(0.0f);

    waitTimer_.Start();
}

void LoadScene::Finalize()
{
    pBar_->Finalize();
    pCanvas_->Finalize();
    pLayer_->RemoveCanvas(pCanvas_.get());
}

void LoadScene::Update()
{
    if (waitTimer_.GetNow<float>() >= kWaitTime_ && !isAggregated_)
    {
        auto& cfg = ConfigManager::GetInstance()->GetConfigData();
        this->AggregateTexturePaths(cfg.texture_paths.front());
        this->AggregateTexturePaths(cfg.model_paths.front());
        pBar_->SetMaxValue(static_cast<float>(texturePaths_.size()));
        isAggregated_ = true;
    }

    if (isAggregated_ && !texturePaths_.empty())
    {
        pTextureManager_->LoadTexture(texturePaths_.back());
        texturePaths_.pop_back();
        current_ += 1.0f;
    }
    
    // スムーズに値を変化させる
    const float barValue = pBar_->GetCurrentValue();
    float smoothValue = barValue + (current_ - barValue) * kSmoothFactor_;

    pBar_->SetCurrentValue(smoothValue);

    if (isAggregated_ && pBar_->GetCurrentValue() >= pBar_->GetMaxValue() - 0.1f && !isChangingScene_)
    {
        SceneManager::GetInstance()->ReserveScene("GameScene", std::make_unique<TransShutter>());
        isChangingScene_ = true;
    }

    pSpriteLBackground_->Update();
    pSpriteLoading_->Update();
    pBar_->Update();
}

void LoadScene::Draw()
{
    CanvasScope canvasScope(pCanvas_.get());
    pSpriteLBackground_->Draw1F();
    pSpriteLoading_->Draw1F();
    pBar_->Draw1F();
}

void LoadScene::AggregateTexturePaths(const std::string& directoryPath)
{
    std::filesystem::path dirPath(directoryPath);
    if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath))
    {
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath))
    {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".png" || ext == ".dds" || ext == ".jpg" || ext == ".jpeg")
        {
            texturePaths_.push_back(entry.path().string());
        }
    }
}
